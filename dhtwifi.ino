#include "DHT.h"
#include <SoftwareSerial.h>

#define DEBUG true
#define ESPRXPIN 4
#define ESPTXPIN 5
#define DHTPIN 8

#define SEND_DATA_COUNT 5

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(ESPRXPIN, ESPTXPIN); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                                        // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                                        // and the RX line from the esp to the Arduino's pin 3

float h;  // humidity
float t;  // Celsius temperature
float f;  // Fahrenheit temperature
float hif;  // computeHeatIndex of Celsius temperature
float hic;  // computeHeatIndex of Fahrenheit temperature

int wifiCount;

void setup() {
  // put your setup code here, to run once:
  wifiCount = 0;
  
  Serial.begin(9600);
  Serial.println("dht sensor & wifi module test.");

  esp8266.begin(9600);

  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CIOBAUD?\r\n",2000,DEBUG); // check baudrate (redundant)
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point (working mode: AP+STA)
  sendData("AT+CWLAP\r\n",3000,DEBUG); // list available access points
  sendData("AT+CWJAP=\"wevo\",\"etcho523\"\r\n",5000,DEBUG); // join the access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  
  dht.begin();
}

void loop() {
  wifiCount++;
  
  if(esp8266.available()) { // check if the esp is sending a message
    if(esp8266.find("+IPD,")) {
      // get the connection id so that we can then disconnect
      int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
      esp8266.find("pin="); // advance cursor to "pin="
      //int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
      //pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
      //digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    

      int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
      pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number

      if(pinNumber == 1) {
        Serial.println("connectionId: "+String(connectionId));

        String sendMsg = "AT+CIPSEND=";
        sendMsg += connectionId;
        sendMsg += ",";
        sendMsg += "10";
        sendMsg += "\r\n";
        sendData(sendMsg,1000,DEBUG);
        String msgBlock = "test123456\r\n";
        sendData(msgBlock,1000,DEBUG);
  
        // make close command
        String closeCommand = "AT+CIPCLOSE="; 
        closeCommand+=connectionId; // append connection id
        closeCommand+="\r\n";
        sendData(closeCommand,1000,DEBUG); // close connection
      }
      else {
        Serial.println("connectionId: "+String(connectionId));
        Serial.println("pinNumber: "+String(pinNumber));
      }
      
    }
  }
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  if (wifiCount == SEND_DATA_COUNT) {
    Serial.println("Send Data To Raspberry Pi Server.");

    sendData("AT+CIPSTART=4,\"TCP\",\"192.168.10.106\",80\r\n",1000,DEBUG);

    String getMethodData = "GET /sensor?ukey=arduino4&sid=dhtsensor1&sdata={";
    getMethodData += String(h);
    getMethodData += ",";
    getMethodData += String(t);
    getMethodData += ",";
    getMethodData += String(f);
    getMethodData += "}\r\n";

    int gLength = getMethodData.indexOf("\n")+1;
    String getMethodLength(gLength);

    sendData("AT+CIPSEND=4,"+getMethodLength+"\r\n",1000,DEBUG);

    sendData(getMethodData,1000,DEBUG);

    sendData("AT+CIPCLOSE=4\r\n",1000,DEBUG);

    wifiCount = 0;
  }
}

/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug) {
    Serial.println(command);
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis()) {
      while(esp8266.available()) {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }
    }
    
    if(debug) {
      Serial.print(response);
    }
    return response;
}

