#include "arduino_mongodb.h"

// --------------------------------------------
// ---------- ArduinoMongoDB ---------------
// --------------------------------------------

ArduinoMongoDB::ArduinoMongoDB()
{
    // Create the ArduinoMongoDB path if it does not exist.
    // This is where all databases are stored
    if(!little_fs.exists(ARDUINO_MONGODB_PATH))
        little_fs.mkdir(ARDUINO_MONGODB_PATH);
}


        // ------------------ DATABASE OPERATIONS ------------------

bool ArduinoMongoDB::connect(const String &db)
{
    String db_name = db;
    if(db_name.startsWith("mongodb://")){
        db_name = db_name.substring(10);
    }
    
    // Check if this database exists before, otherwise initialize a new database
    bool success = true;
    if(!little_fs.exists(String(ARDUINO_MONGODB_PATH) + "/" + db_name))
        success = little_fs.mkdir(String(ARDUINO_MONGODB_PATH) + "/" + db_name);

    if (success){
        _currentURI = String(ARDUINO_MONGODB_PATH) + "/" + db_name + "/";
    }
    return success;
}

String ArduinoMongoDB::currentDatabase()
{
    if (connected())
        return _currentURI.substring(String(ARDUINO_MONGODB_PATH).length());
    else
        return "";
}


        // ------------------ COLLECTION OPERATIONS ------------------
bool ArduinoMongoDB::createCollection(const String &collection_name)
{
    if(!connected())
        return false;
    
    // Check if this collection exists before, otherwise initialize a new collection
    bool success = true;
    if(!little_fs.exists(String(_currentURI) + collection_name))
        success = little_fs.mkdir(String(_currentURI) + collection_name);
    
    // TODO: Add necessary indexes, and other config. files to the collection
    return success;
}

bool ArduinoMongoDB::deleteCollection(const String &collection_name)
{
    if(!connected())
        return false;
    
    // Remove the collection directory if it exists
    return little_fs.rmdir(String(_currentURI) + collection_name);
}


        // ------------------ DOCUMENT OPERATIONS ------------------
bool ArduinoMongoDB::createDocument(const String &document, const String &collection, const String &ID)
{
    if(!connected())
        return false;
    
    // Check if the collection exists. Return false if it does not.
    if(!little_fs.exists(String(_currentURI) + collection))
        return false;

    // Create the document file
    return little_fs.writeFile(docFilename(collection, ID), document);
}

String ArduinoMongoDB::readDocument(const String &collection, const String &ID)
{
    if(!connected())
        return "";
    
    // Read the document file
    return little_fs.readFile(docFilename(collection, ID));
}

template <typename T>
void ArduinoMongoDB::findDocuments(const String &collection, T callback)
{
    if(!connected())
        return;
    
    // Check if the collection exists. Return false if it does not.
    if(!little_fs.exists(String(_currentURI) + collection))
        return;
    
    // Find all documents in the collection
    Dir dir = little_fs.openDir(String(_currentURI) + collection);
    while(dir.next()){
        // Call the callback function with the document
        callback(little_fs.readFile(dir.fileName()));
    }
}

bool ArduinoMongoDB::updateDocument(const String &document, const String &collection, const String &ID)
{
    return createDocument(document, collection, ID);
}

bool ArduinoMongoDB::deleteDocument(const String &collection, const String &ID)
{
    if(!connected())
        return false;
    
    // Check if the collection exists. Return false if it does not.
    if(!little_fs.exists(String(_currentURI) + collection))
        return false;
    
    // Remove the document file
    return little_fs.remove(docFilename(collection, ID));
}