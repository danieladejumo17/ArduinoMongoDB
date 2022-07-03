// ######################################
// -------- ARDUINO MONGO DB ---------
// ######################################

#include "schema.h"

void setup()
{
    Serial.begin(115200);

    using SF = ArduinoMongoSchemaField;
    auto userSchema = ArduinoMongoSchema({{"name", SF(DBType::Str, true, "", 0, 0, 
                                            [](const String& name){
                                                return name.length() > 3;
                                            })},
                                          {"age", SF(DBType::Int, true, "", 0, 200, nullptr)},
                                          {"height", SF(DBType::Float, true, "", 0, 10, nullptr)},
                                          {"weight", SF(DBType::Double, true, "", 0, 200, nullptr)},
                                          {"is_admin", SF(DBType::Boolean, true, "", 0, 0, nullptr)},
                                          {"role", SF(DBType::Object, false, "{}", 0, 0, nullptr)}});

    // Create a string document matching the schema, omiting the `role` field
    // "name": "John",
    // "age": 30,
    // "height": 1.75,
    // "weight": 75.0,
    // "is_admin": true
    String doc = "{"
                 "\"name\": \"John\","
                 "\"age\": 30,"
                 "\"height\": 1.75,"
                 "\"weight\": 75.0,"
                 "\"is_admin\": true"
                 "}";

    // use userSchema to verify the document
    if (userSchema.verifyDocument(doc))
    {
        Serial.println("Document is valid");
    }
    else
    {
        Serial.println("Document is invalid");
    }

    // fill default values for the document using the schema
    doc = userSchema.fillDefaultValues(doc);
    Serial.println(doc);
}

void loop()
{
}