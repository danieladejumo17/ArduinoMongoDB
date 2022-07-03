#ifndef SCHEMA_HEADER
#define SCHEMA_HEADER

#include <Arduino.h>

struct ArduinoMongoSchemaField
/* Defines properties of each field in a DB document:
 * - `Type` of the data stored in the field
 * - `Required`
 * - `Default` value
 * - `Min`, `Max` values for numeric fields
 * - Validation function for custom validation
 * */
{
    enum Type: int {Str, Int, Float, Double, Boolean, Object};
    // Type::String, and String are conflicting here

    Type type;
    bool required;
    String defaultValue; //TODO: use union to specify default type field
    int min;
    int max;
    bool (*validation)(const String&); 
};
using DBType = ArduinoMongoSchemaField::Type;


#endif