#include "schema.h"


// ######################################
// -------------- SCHEMA ----------------
// ######################################

bool ArduinoMongoSchema::verifyDocument(const String& doc) const
{
    const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + _schemaBufferSize;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& json = jsonBuffer.parseObject(doc);

    if(!json.success())
    {
        logerr("Failed to parse JSON document");
        return false;
    }

    // iterate over all fields in the schema
    for(auto field: _schema){
        // If a field is required, confirm key exists in doc 
        if(field.second.required){
            bool exists = json.containsKey(field.first);
            if(!exists)
            {
                logerr("Validation of field " + field.first + " failed: field is required");
                return false;
            }
                
        }

        // For all keys that exist in the document confirm they have the right type
        // Fields that are not required can be missing from the document
        if(json.containsKey(field.first)){
            if(!checkDataConversion(json[field.first], field.second.type))
            {
                logerr("Validation of field " + field.first + " failed: wrong type");
                return false;
            }
        }

        // If a field min/max !infinity() and is a numeric type, validate that the value is within the range
        if(field.second.min != -infinity() || field.second.max != infinity()){
            if(field.second.type == DBType::Int || field.second.type == DBType::Float || field.second.type == DBType::Double){
                if(json[field.first].as<double>() < field.second.min || json[field.first].as<double>() > field.second.max)
                {
                    logerr("Validation of field " + field.first + " failed: Value is out of range.");
                    return false;
                }
            }
        }

        // If a field has a validation function, call it and confirm it returns true
        if(field.second.validation != nullptr){
            if(!field.second.validation(json[field.first]))
            {
                logerr("Validation of field " + field.first + " failed: Validation function returned false");
                return false;
            }
        }
    }

    return true;
}

String ArduinoMongoSchema::fillDefaultValues(const String& doc) const
{
    const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + _schemaBufferSize;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& json = jsonBuffer.parseObject(doc);

    if(!json.success())
        return "";

    // iterate over all fields in the schema
    for(auto field: _schema){
        // If a field has a default value, fill it in if it is missing
        if(field.second.defaultValue != ""){
            if(!json.containsKey(field.first))
                json[field.first] = field.second.defaultValue;
        }
    }

    String res = "";
    json.printTo(res);
    return res;
}

String trimZeros(const String& str)
{
    String res = str;

    // Trim trailing zeros after the decimal point
    size_t i = res.indexOf('.');
    if(i != -1)
    {
        size_t j = res.length() - 1;
        while(res[j] == '0')
            j--;
        if(res[j] == '.')
            j--;
        res = res.substring(0, j + 1);
    }

    // Trim leading zeros
    i = 0;
    while(res[i] == '0')
        i++;
    if(i != 0)
        res = res.substring(i);

    return res;
}

bool ArduinoMongoSchema::checkDataConversion(const String& str, DBType type) const
{
    // lambda function to compare two strings after trimming zeros
    auto eqNums = [](const String& a, const String& b){
        return trimZeros(a) == trimZeros(b);
    };

    switch (type)
    {
    case DBType::Str:
        return true;
    case DBType::Int:
        if(String(str.toInt()) != str){
            logwarn("Schema Error: Error in converting field value `" + str + "` to type `int`");
            return false;
        }
        return true;
    case DBType::Float:
        if(!eqNums(String(str.toFloat()), str)){
            logwarn("Schema Error: Error in converting field value `" + str + "` to type `float`");
            return false;
        }
        return true;
    case DBType::Double:
        if(!eqNums(String(str.toDouble()), str)){
            logwarn("Schema Error: Error in converting field value `" + str + "` to type `double`");
            return false;
        }
        return true;
    case DBType::Boolean:
        if(!(str.equalsIgnoreCase("true") || str.equalsIgnoreCase("false"))){
            logwarn("Schema Error: Error in converting field value `" + str + "` to type `boolean`");
            return false;
        }
        return true;
    case DBType::Object:
        const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + _schemaBufferSize;
        DynamicJsonBuffer jsonBuffer(capacity);
        JsonObject& json = jsonBuffer.parseObject(str);

        if(!json.success()){
            logwarn("Schema Error: Error in converting field value `" + str + "` to type `object`");
            return false;
        }
        return true;
    }
}


// ######################################
// -------------- UTILITIES ----------------
// ######################################
// define a split function for type String. Returns a vector of strings.
/*
std::vector<String> split(const String& str, char delimiter) {
    std::vector<String> internal;
    String current;
    for (char c : str) {
        if (c == delimiter) {
            internal.push_back(current);
            current = "";
        } else {
            current += c;
        }
    }
    internal.push_back(current);
    return internal;
}


// a function that parses a variable number of string arguments to a ArduinoMongoSchemaField object
// String arguments are in the form of "name1: value1, name2: value2, ...".
ArduinoMongoSchemaField SF(const String& str, bool (*validation)(const String&) = nullptr)
{
    ArduinoMongoSchemaField field;
    auto args = split(str, ',');
    for(auto& arg: args)
    {
        auto kvp = split(arg, ':');
        if(kvp.size() != 2)
            continue;
        auto key = kvp[0];
        auto value = kvp[1];
        // trim whitespace from key and value
        key.trim();
        value.trim();

        if(key == "type")
        {
            if(value == "str")
                field.type = ArduinoMongoSchemaField::Type::Str;
            else if(value == "int")
                field.type = ArduinoMongoSchemaField::Type::Int;
            else if(value == "float")
                field.type = ArduinoMongoSchemaField::Type::Float;
            else if(value == "double")
                field.type = ArduinoMongoSchemaField::Type::Double;
            else if(value == "bool")
                field.type = ArduinoMongoSchemaField::Type::Boolean;
            else if(value == "object")
                field.type = ArduinoMongoSchemaField::Type::Object;
            else
                continue;
        }
        else if(key == "required")
        {
            if(value == "true")
                field.required = true;
            else if(value == "false")
                field.required = false;
            else
                continue;
        }
        else if(key == "default")
        {
            field.defaultValue = value;
        }
        else if(key == "min")
        {
            field.min = value.toDouble();
        }
        else if(key == "max")
        {
            field.max = value.toDouble();
        }
    }

    // if a validation function is provided, set it
    if(validation != nullptr)
        field.validation = validation;

    return field;
}
*/