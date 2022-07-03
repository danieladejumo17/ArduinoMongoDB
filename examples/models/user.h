#ifndef USER_MODEL_HEADER
#define USER_MODEL_HEADER

#include "../ArduinoMongoDB.h"

auto createAdminUser = [](ArduinoMongoModel User){
    // If an admin user does not exist in the database already, create one
};

auto userSchema = ArduinoMongoSchema({ 
  { "name", {DBType::String, true}},
  {"password", {DBType::String, true}},
  {"house", {DBType::String, true}}
});

// TODO: Make the find and createNew and other like functions const
static const auto User = ArduinoMongoDB::model("User", userSchema);

createAdminUser(User);

#endif