#ifndef ARDUINO_MONGO_DB_HEADER
#define ARDUINO_MONGO_DB_HEADER

#include "littlefs_filesystem.h"
#include "arduino_utilities.h"

// File IO interface for the DB
#define ARDUINO_MONGODB_PATH "/AMDB"
class ArduinoMongoDB{
    private:
        static String _currentURI;

        /** 
         * docFilename(collection, ID)
         * Returns the filename of document with id `ID` inside the collection `collection`
         * */
        static String docFilename(const String& collection, const String& ID) 
        {
            return String(_currentURI) + collection + "/" + ID;
        }

    public:
        ArduinoMongoDB();

        // ------------------ DATABASE OPERATIONS ------------------
        // Connects to a database. Database URI specified as -> "mongodb://MyApp"
        static bool connect(const String&);

        // Returns true if the database is connected.
        static bool connected() {return !_currentURI.isEmpty();}

        // Returns the current database URI.
        static String currentDatabase();


        // ------------------ COLLECTION OPERATIONS ------------------
        // Create a new collection in the current database.
        static bool createCollection(const String&);

        // Delete a collection from the current database.
        static bool deleteCollection(const String&);


        // ------------------ DOCUMENT OPERATIONS ------------------
        /**
         * createDocument(document, collection, ID)
         * Creates a new document in the specified collection.
         * :param document: The document String to create.
         * :param collection: The collection to create the document in.
         * :param ID: The ID of the document.
         * */
        static bool createDocument(const String&, const String&, const String&);

        /**
         * readDocument(collection, ID)
         * Reads document with the specified id from the specified collection.
         * :param collection: The collection to create the document in.
         * :param ID: The ID of the document.
         * */
        static String readDocument(const String&, const String&);

        /**
         * findDocuments(collection, callback)
         * Finds all documents in the specified collection that match the callback function.
         * :param collection: The collection to get documents from.
         * :param callback: The callback function to use to find documents.
         * */
        template <typename T>
        static void findDocuments(const String&, T);

        /**
         * updateDocument(document, collection, ID)
         * Updates the document with the specified ID in the specified collection. The document will be replaced with the new document.
         * :param collection: The collection to update the document in.
         * :param ID: The ID of the document to update.
         * :param document: The document to overwrite with.
         * */
        static bool updateDocument(const String&, const String&, const String&);

        /**
         * deleteDocument(collection, ID)
         * Deletes the document with the specified ID from the specified collection.
         * :param collection: The collection to delete the document from.
         * :param ID: The ID of the document to delete.
         * */
        static bool deleteDocument(const String&, const String&);

};
#endif


/*
    Database Folder
    ---> Collection Folder
         ---> Document Files

*/