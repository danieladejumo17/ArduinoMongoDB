#ifndef SCHEMA_HEADER
#define SCHEMA_HEADER

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include "arduino_utilities.h"


// ######################################
// ------------ SCHEMA FIELD ------------
// ######################################

/* Defines properties of each field in a DB document:
 * - `Type` of the data stored in the field
 * - `Required`
 * - `Default` value
 * - `Min`, `Max` values for numeric fields
 * - Validation function for custom validation
 * */
struct ArduinoMongoSchemaField
{
    enum Type: int {Str, Int, Float, Double, Boolean, Object};
    // Type::String, and String are conflicting here

    Type type = Type::Str;
    bool required = false;
    String defaultValue = ""; //TODO: use union to specify default type field
    double min = -infinity();
    double max = infinity();
    bool (*validation)(const String&) = nullptr; 
    // TODO: see how templates can work with this

    // constructor
    ArduinoMongoSchemaField(Type type, bool required, const String& defaultValue, double min, double max, bool (*validation)(const String&))
    {
        this->type = type;
        this->required = required;
        this->defaultValue = defaultValue;
        this->min = min;
        this->max = max;
        this->validation = validation;
    }

    // TODO: define a constructor that takes a string argument
};
using DBType = ArduinoMongoSchemaField::Type;



// ######################################
// -------------- SCHEMA ----------------
// ######################################

/* Defines the structure of a document in the DB.
 * - `_schema` is a map of field names to their properties `ArduinoMongoSchemaField`
 * - `_schemaBufferSize` is the size of the buffer used to store the JSON document
 * */
struct ArduinoMongoSchema
{
    ArduinoMongoSchema(const std::map<String, ArduinoMongoSchemaField>& schema)
        : _schema{schema}
        {}

    // Returns true if the document is valid according to the schema
    bool verifyDocument(const String&) const;

    /* Returns a document with default values for all missing fields */
    String fillDefaultValues(const String&) const;

    private:
        const std::map<String, ArduinoMongoSchemaField> _schema;
        bool checkDataConversion(const String& str, DBType type) const;
        size_t _schemaBufferSize = 200;
        // TODO: See how DynamicJsonBuffer works with this
};

#endif