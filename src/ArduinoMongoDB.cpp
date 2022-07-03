#include "ArduinoMongoDB.h"
#include "HTTP_Utilities.h"

bool ArduinoMongoSchema::verifySchema(const String& doc) const
{
    const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + _schemaBufferSize;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& json = jsonBuffer.parseObject(doc);

    if(!json.success())
        return false;


    for(auto i: _schema){
        bool exists = false;

        // If key is required, confirm key exists in doc 
        if(i.second.required){
            exists = json.containsKey(i.first);
            if(!exists) return false;
        }

        // For all keys that exist in the document confirm they have the right type
        if(exists || json.containsKey(i.first)){
            if(!checkCast(json[i.first], i.second.type))
                return false;
        }
    }

    return true;
}

bool ArduinoMongoSchema::checkCast(const String& str, DBType type) const
{
    switch (type)
    {
    case DBType::String:
        return true;
    case DBType::Int:
        if(String(str.toInt()) != str){
            logWarn("Schema Error: Error in converting value to type 'int'");
            return false;
        }
        return true;
    case DBType::Float:
        if(String(str.toInt()) != str){
            logWarn("Schema Error: Error in converting value to type 'float'");
            return false;
        }
        return true;
    case DBType::Double:
        if(String(str.toDouble()) != str){
            logWarn("Schema Error: Error in converting value to type 'double'");
            return false;
        }
        return true;
    case DBType::Boolean:
        if(str != "true" || str != "false"){
            logWarn("Schema Error: Error in converting value to type 'boolean'");
            return false;
        }
        return true;
    case DBType::Object:
        const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + _schemaBufferSize;
        DynamicJsonBuffer jsonBuffer(capacity);
        JsonObject& json = jsonBuffer.parseObject(str);

        if(!json.success()){
            logWarn("Schema Error: Error in converting value to type 'object'");
            return false;
        }
        return true;
    }
}



// --------------------------------------------
// ---------- ArduinoMongoModel ---------------
// --------------------------------------------

template<typename Callback>
void ArduinoMongoModel::find(bool(*match)(const ArduinoMongoModel&), Callback callback)
{
    auto doc = createNew();
    bool found = false;
    ArduinoMongoDB::findDocument(_collection, [&](const String& docString)->bool{
        auto temp = createNew();
        if(!temp.set(docString)) return false;

        found = match(temp);
        if(found)
            doc = temp; // doc is only set if found

        return found;
    },
    [&](bool err, const char* msg){
        const char* errMsg = err? msg:(found? "":"No match found for requested document");
        callback(errMsg, err || (!found)? createNew():doc); // doc is only set if found
        // deals with the error of mistakenly using doc though an error was reported
    });

}

template<typename Callback>
void ArduinoMongoModel::find(const String &id, Callback callback)
{
    ArduinoMongoDB::findDocumentById(_collection, id, [&](bool err, const char* msg, const String& docString){
        callback(err?msg:"", err?createNew():createNew(docString));
    });
}

const ArduinoMongoModel& ArduinoMongoModel::createNew() const
{
    return ArduinoMongoModel{_collection, _schema};
}

const ArduinoMongoModel& ArduinoMongoModel::createNew(const String &docString) const
{
    ArduinoMongoModel newDoc = createNew();
    newDoc.set(docString);
    return newDoc;
}

bool ArduinoMongoModel::set(const String &docString)
{   
    // FIXME:
    // clear the buffer first???
    _jsonBuffer.clear();

    // FIXME:
    // _doc = _jsonBuffer.parseObject(docString.c_str()); assignment operator deleted
    // hopefully parsing a new object in the jsonBuffer updates the reference
    _jsonBuffer.parseObject(docString.c_str());

    return _doc.success();
}

template<typename Callback>
void ArduinoMongoModel::save(Callback callback)
{
    // TODO: if verifySchema(doc) -> the one taking a doc argument
    // User can create _id field in schema and it will not be filled anymore here
    if(!_doc.containsKey("_id")) // TODO: and _id has not been requested to be off
        (*this)["_id"] = ArduinoMongoDB::createDocId(_collection);

    ArduinoMongoDB::saveDocument(_collection, _doc, [&](bool err, const char* msg){
        callback(err? msg:"", *this);
    });
}

template<typename Callback>
void ArduinoMongoModel::remove(Callback callback)
{
    if(_doc.containsKey("_id")){ // if the doc is saved
        ArduinoMongoDB::removeDocument(_collection, _doc["_id"], [&](bool err, const char* msg){
            callback(err?msg:"", *this);
        });
    }
}


// --------------------------------------------
// ---------- ArduinoMongoDB ---------------
// --------------------------------------------

ArduinoMongoDB::ArduinoMongoDB()
{
    if(!localStorage.exists(ARDUINO_MONGODB_PATH))
        localStorage.mkdir(ARDUINO_MONGODB_PATH);
}

bool ArduinoMongoDB::initCollection(const String &collection)
{
    if(connected())
    {
        if(collectionExists(collection)){
            return true;
        }else if(localStorage.mkdir(collectionPath(collection))){
            // TODO: Poupulate collection init file properly
            return localStorage.writeFile(documentPath(collection, ".mdbc"), "");
        }
    }

    return false;
}

bool ArduinoMongoDB::collectionExists(const String &collection)
{
    // TODO: confirm if this is a valid collection dir
    return localStorage.exists(collectionPath(collection));
}

bool ArduinoMongoDB::documentExists(const String &collection, const String &docID)
{
    // TODO: confirm if this is a valid document - nothing much here
    return localStorage.exists(documentPath(collection, docID));
}


String ArduinoMongoDB::collectionPath(const String &collection)
{
    return String(ARDUINO_MONGODB_PATH) + "/" + _currentDatabase + "/" + collection;
}

String ArduinoMongoDB::documentPath(const String &collection, const String &docID)
{
    return collectionPath(collection) + "/" + docID + ".doc";
}

void ArduinoMongoDB::connect(const String &db)
{
    // TODO: Check if this database exists beofore, otherwise initialize a new database
    // TODO: ensure connected() returns the right state of the database
    _currentDatabase = db;
}

ArduinoMongoModel ArduinoMongoDB::model(const String &collection, const ArduinoMongoSchema &schema)
{
    if(!initCollection(collection))
        logErr(String("ERROR: Error initializing model '") + collection + "'.");
        
    return ArduinoMongoModel(collection, schema);
}

String ArduinoMongoDB::createDocId(const String&)
{

}

template<typename Callback>
bool ArduinoMongoDB::validDBOperation(const String &collection, Callback callback)
{
    if(!connected()){
        callback(true, "Database opeartion failed. No database connected to");
        return false;
    }

    if(!collectionExists(collection)){
        callback(true, "Database operation failed. Collection does not exist");
        return false;
    }

    return true;
}


void ArduinoMongoDB::saveDocument(const String &collection, const JsonObject &doc, void(*callback)(bool, const char*))
{
    if(!validDBOperation(collection, callback))
        return;

    if(doc["_id"]){
        String docString;
        doc.printTo(docString);

        if(localStorage.writeFile(documentPath(collection, doc["_id"]), docString))
            callback(false, "");
        else
            callback(true, "IO Error. Failed to write changes to database");
    }else{
        callback(true, "Save failed. Document does not have a valid id");
    }
}

void ArduinoMongoDB::findDocument(const String &collection, bool(*matchCallback)(const String&), void(*callback)(bool, const char*))
{
    // Handles database exists and collection exists errors
    if(!validDBOperation(collection, callback))
        return;
    
    Dir dir = localStorage.openDir(collectionPath(collection));

    auto isDoc = [](const String& filename){
        return filename.endsWith(".doc");
    };

    while(dir.next()){
        if(dir.fileSize() && isDoc(dir.fileName())){
            if(matchCallback(localStorage.readFile(dir.fileName())))
                break;
        }
    }

    callback(false, "");
}

void ArduinoMongoDB::findDocumentById(const String &collection, const String &id, void(*callback)(bool, const char*, const String&))
{
    if(!validDBOperation(collection, [&](bool err, const char* errMsg){
        callback(err, errMsg, "");
    })){
        return;
    }

    String docPath  = documentPath(collection, id);
    if(!localStorage.exists(docPath)){
        callback(true, "Document does not exist in database", "");
        return;
    }

    String docString = localStorage.readFile(docPath);
    callback(docString, docString?"":"IO Error. Unable to retrieve file from database", docString);
}

void ArduinoMongoDB::removeDocument(const String &collection, const String &id, void(*callback)(bool, const char*)) // collection, id
{
    if(!validDBOperation(collection, callback))
        return;

    String docPath  = documentPath(collection, id);
    if(!localStorage.exists(docPath)){
        callback(true, "Document does not exist in database");
        return;
    }

    if(!localStorage.remove(docPath)){
        callback(true, "IO Error. Unable to remove file from database");
        return;
    }

    callback(false, "");
}







 