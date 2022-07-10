// // --------------------------------------------
// // ---------- ArduinoMongoModel ---------------
// // --------------------------------------------

// template<typename Callback>
// void ArduinoMongoModel::find(bool(*match)(const ArduinoMongoModel&), Callback callback)
// {
//     auto doc = createNew();
//     bool found = false;
//     ArduinoMongoDB::findDocument(_collection, [&](const String& docString)->bool{
//         auto temp = createNew();
//         if(!temp.set(docString)) return false;

//         found = match(temp);
//         if(found)
//             doc = temp; // doc is only set if found

//         return found;
//     },
//     [&](bool err, const char* msg){
//         const char* errMsg = err? msg:(found? "":"No match found for requested document");
//         callback(errMsg, err || (!found)? createNew():doc); // doc is only set if found
//         // deals with the error of mistakenly using doc though an error was reported
//     });

// }

// template<typename Callback>
// void ArduinoMongoModel::find(const String &id, Callback callback)
// {
//     ArduinoMongoDB::findDocumentById(_collection, id, [&](bool err, const char* msg, const String& docString){
//         callback(err?msg:"", err?createNew():createNew(docString));
//     });
// }

// const ArduinoMongoModel& ArduinoMongoModel::createNew() const
// {
//     return ArduinoMongoModel{_collection, _schema};
// }

// const ArduinoMongoModel& ArduinoMongoModel::createNew(const String &docString) const
// {
//     ArduinoMongoModel newDoc = createNew();
//     newDoc.set(docString);
//     return newDoc;
// }

// bool ArduinoMongoModel::set(const String &docString)
// {   
//     // FIXME:
//     // clear the buffer first???
//     _jsonBuffer.clear();

//     // FIXME:
//     // _doc = _jsonBuffer.parseObject(docString.c_str()); assignment operator deleted
//     // hopefully parsing a new object in the jsonBuffer updates the reference
//     _jsonBuffer.parseObject(docString.c_str());

//     return _doc.success();
// }

// template<typename Callback>
// void ArduinoMongoModel::save(Callback callback)
// {
//     // TODO: if verifySchema(doc) -> the one taking a doc argument
//     // User can create _id field in schema and it will not be filled anymore here
//     if(!_doc.containsKey("_id")) // TODO: and _id has not been requested to be off
//         (*this)["_id"] = ArduinoMongoDB::createDocId(_collection);

//     ArduinoMongoDB::saveDocument(_collection, _doc, [&](bool err, const char* msg){
//         callback(err? msg:"", *this);
//     });
// }

// template<typename Callback>
// void ArduinoMongoModel::remove(Callback callback)
// {
//     if(_doc.containsKey("_id")){ // if the doc is saved
//         ArduinoMongoDB::removeDocument(_collection, _doc["_id"], [&](bool err, const char* msg){
//             callback(err?msg:"", *this);
//         });
//     }
// }

