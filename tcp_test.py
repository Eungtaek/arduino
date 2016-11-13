from socket import *

HOST = '192.168.10.106'
PORT = 9023
BUFSIZE = 1024

serverSock = socket(AF_INET, SOCK_STREAM)

serverSock.bind((HOST, PORT))

serverSock.listen(5)

conn, addr = serverSock.accept()

conn.recv(BUFSIZE)

conn.send("OK")

conn.close()

serverSock.close()
