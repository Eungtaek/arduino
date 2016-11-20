import pymongo

connection = pymongo.MongoClient("localhost",27017)

db = connection.AAA
collection = db.testCollection

collection.insert({"number":0})

collection.update({"number":0},{"number":9999})

collectionInfo = db.collection_names()

print collectionInfo

collection = db.student

for i in range(0,100):
    collection.insert({"student_id":i})

docs = collection.find()

for i in docs:
    print i

docs = collection.find({"student_id":{"$gt":90}})

for i in docs:
    print i

collection.remove({"student_id":{"$lt":90}})

docs = collection.find()

for i in docs:
    print i


