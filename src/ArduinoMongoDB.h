#ifndef ARDUINO_MONGO_DB_HEADER
#define ARDUINO_MONGO_DB_HEADER

// TODO: confirm uuid length is not too long for filenames
// FIXME: M,MS,DM,SDMSD,MD,SMDS,DMSDSMD,SMDS,DMSD,SMDSD
#include <map>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "LocalStorage.h"

#define ARDUINO_MONGODB_PATH "/AMDB"

// TODO: calculate _schemaBufferSize in Schema Object
// TODO: Populate fields in models with default values
// TODO: Add timestamp to schema
// TODO: Populate collection init file (.mdbc) properly - creation date,
        // last edit, docs count, and other collection metadata
// TODO: indexing the database collections by _id and other custom indices for fast lookup

// TODO: Update schema field to include ObjectId and its ref

struct ArduinoMongoSchemaField
{
    enum Type: int {String, Int, Float, Double, Boolean, Object};

    Type type;
    bool required;
    // use union to specify default type field
};
using DBType = ArduinoMongoSchemaField::Type;


struct ArduinoMongoSchema
{
    ArduinoMongoSchema(const std::map<String, ArduinoMongoSchemaField>& schema)
        : _schema{schema}, _schemaBufferSize{200}
        // TODO: calculate _schemaBufferSize to initialize properly
        {}
    bool verifySchema(const String&) const;

    // void* operator[](const String&);

    int bufferSize() const {return _schemaBufferSize;}

    private:
        const std::map<String, ArduinoMongoSchemaField> _schema;
        const int _schemaBufferSize;
        // std::map<String, void*> _methods;

        bool checkCast(const String& str, DBType type) const;

};


class ArduinoMongoModel{
    private:
        const String _collection;
        DynamicJsonBuffer _jsonBuffer;
        JsonObject &_doc;
        const ArduinoMongoSchema &_schema;


    public:
        ArduinoMongoModel(const String &collection, const ArduinoMongoSchema &schema)
            : _collection{collection}, _schema{schema}, _jsonBuffer{schema.bufferSize()}, _doc(_jsonBuffer.createObject())
        {}

        ArduinoMongoModel(const ArduinoMongoModel &model)
            :_collection{model._collection}, _schema{model._schema}, _jsonBuffer{model._schema.bufferSize()}, _doc(_jsonBuffer.createObject())
        {}

        template<typename Callback>
        void find(bool(*)(const ArduinoMongoModel&), Callback);
        template<typename Callback>
        void find(const String&, Callback);

        const ArduinoMongoModel& createNew() const;
        const ArduinoMongoModel& createNew(const String&) const;
        ArduinoJson::Internals::JsonObjectSubscript<const String &> operator[](const String &key) {return _doc[key];}

        bool set(const String&);

        template<typename Callback>
        void save(Callback);

        template<typename Callback>
        void remove(Callback);



};


// IDs generated in the ArduinoMomgoDB is only unique on the same machine and not across machines
class ArduinoMongoDB{
    // TODO: Check the initialization of the collections and database before returning true on exists() methods
    private:
        static String _currentDatabase;
        static bool initCollection(const String&); // creates a new collection. called from model()
        static bool collectionExists(const String&);
        static String collectionPath(const String&);

        static bool documentExists(const String&, const String&);
        static String documentPath(const String&, const String&);

    public:
        ArduinoMongoDB();

        
        // Connects the database to a single database at a time
        static void connect(const String&);
        static bool connected() {return _currentDatabase;}

        // Inits a new collection in the current database
        static ArduinoMongoModel model(const String&, const ArduinoMongoSchema& ); // Inits a new collection 

        static String createDocId(const String&); //collection
        
        template<typename Callback>
        static bool validDBOperation(const String&, Callback);
        static void saveDocument(const String&, const JsonObject &doc, void(*)(bool, const char*)); // collection, fileContent
        
        // bool matchCallback(const String &docString)
        // void callback(bool err, const char* errMsg)
        static void findDocument(const String &collection, bool(*matchCallback)(const String&), void(*callback)(bool, const char*)); // collection, mathCallback, callback
        
        // void callback(bool err, const char* errMsg, const String& docString)
        static void findDocumentById(const String &collection, const String &id, void(*callback)(bool, const char*, const String&));
        
        // void callback(bool err, const char* errMsg)
        static void removeDocument(const String &collection, const String &id, void(*callback)(bool, const char*)); // collection, id
};


#endif



/*
    Database Folder
    ---> Collection Folder
         ---> Document Files

*/
