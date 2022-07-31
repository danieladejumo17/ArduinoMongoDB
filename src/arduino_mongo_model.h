#ifndef ARDUINO_MNGO_MODEL_HEADER
#define ARDUINO_MNGO_MODEL_HEADER

#include <Arduino.h>
#include <ArduinoJson.h>
#include "schema.h"
#include "arduino_mongodb.h"
#include "arduino_utilities.h"

// Programming Interface to the DB
class ArduinoMongoModel
{
private:
    const String _collection;
    const ArduinoMongoSchema &_schema;
    String _document;
    bool (*_verify)(const ArduinoMongoModel &);

    /**
     * @returns the next _id in this collection
     */
    String nextID() const;

public:
    ArduinoMongoModel(const String &collection, const ArduinoMongoSchema &schema,
                      bool (*verify)(const ArduinoMongoModel &),
                      const String &document = String());

    ArduinoMongoModel(const String &collection, const ArduinoMongoSchema &schema,
                      const String &document = String())
        : ArduinoMongoModel{collection, schema, nullptr, document}
    {
    }

    ArduinoMongoModel(const ArduinoMongoModel &model, const String &document = String())
        : ArduinoMongoModel{model._collection, model._schema, model._verify, document}
    {
    }

    // -------------- CREATE & UPDATE OPERATIONS --------------

    /**
     * @brief Sets the document to the provided JSON string.
     * @param document JSON string of the document to create
     */
    void setDocument(const String &docString);

    /**
     * @brief Sets the value of 'key' to the provided 'value'.
     * @param key key to set
     * @param value value to set the key to
     */
    void set(const String &key, const String &value);

    /**
     * @brief Saves this document to the database
     * @returns true if operation is successful, otherwise false
     */
    bool save();

    /**
     * @brief Saves this document to the database
     * @param callback a callable function that takes `doc` and `err` parameters.
     * `doc` is a String and `err` is a boolean
     */
    template <typename Callback>
    void save(Callback callback);

    // -------------- READ OPERATIONS --------------

    /**
     * @brief Gets the string value of a key
     * @param key key to get from the document
     * @returns value of the key or empty string if key does not exist
     */
    String get(const String &) const;

    /**
     * @brief Gets the string value of a key.
     * This is an alias of ArduinoMongoModel::get
     * @returns value of the key or empty string if key does not exist
     */
    String operator[](const String &) const;

    /**
     * @returns the String representation of this document
     */
    operator String() const;

    /**
     * @returns the String representation of this document
     */
    String toString() const;

    /**
     * @brief Find a document by its ID
     * @param _id document's ID
     * @param callback a callable function that takes `doc` and `err` parameters.
     * `doc` is a String of the document found, it's empty if document is not found.
     * `err` is a boolean, it's true if the operation fails
     */
    template <typename Callback>
    void findByID(const String &_id, Callback callback);

    /**
     * @brief Find a document by a custom function.
     * @param find_cb this function is called with a ArduinoMongoModel object of each document
     * in this collection. It should return true for a matching document. Only the first document
     * is reckoned with.
     * @param callback a callable function that takes `doc` and `err` parameters.
     * `doc` is a String of the document found, it's empty if no match is found.
     * `err` is a boolean, it's true if the operation fails
     */
    template <typename Callback>
    void find(bool (*find_cb)(const ArduinoMongoModel &), Callback callback);

    /**
     * @brief Find a document by a custom function.
     * @param find_cb this function is called with a JSON String of each document in this
     * collection. It should return true for a matching document. Only the first document is
     * reckoned with.
     * @param callback a callable function that takes `doc` and `err` parameters.
     * `doc` is a String of the document found, it's empty if no match is found.
     * `err` is a boolean, it's true if the operation fails
     */
    template <typename Callback>
    void find(bool (*find_cb)(const String &), Callback callback);

    // -------------- DELETE OPERATION --------------

    /**
     * @brief deletes this document from the database
     * @returns true if operation is successful, otherwise false
     */
    bool remove();

    /**
     * @brief deletes this document from the database
     * @param callback a callable function that takes `doc` and `err` parameters.
     * `doc` is a String of this document
     * `err` is a boolean, it's true if the operation fails
     */
    template <typename Callback>
    void remove(Callback);
};

#endif // ARDUINO_MNGO_MODEL_HEADER