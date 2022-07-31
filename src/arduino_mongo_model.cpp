// // --------------------------------------------
// // ---------- ArduinoMongoModel ---------------
// // --------------------------------------------
#include "arduino_mongo_model.h"

ArduinoMongoModel::ArduinoMongoModel(const String &collection, const ArduinoMongoSchema &schema,
                                     bool (*verify)(const ArduinoMongoModel &),
                                     const String &document = String())
    : _collection{collection}, _schema{schema}, _document{document}, _verify{verify}
{
    // Initialize the collection if it doesn't exist
    if (!ArduinoMongoDB::connected())
    {
        logerr("Failed to initialize collection: database is not connected");
        return;
    }

    if (!ArduinoMongoDB::createCollection(_collection))
    {
        logerr("Failed to initialize collection: failed to create collection");
        return;
    }
}

void ArduinoMongoModel::setDocument(const String &docString)
{
    _document = docString;
}

void ArduinoMongoModel::set(const String &key, const String &value)
{
    deserializeJSON(_document, [&](JsonObject &json)
                    {
        json[key] = value;
        _document = serializeJSON(json); });
}

bool ArduinoMongoModel::save()
{
    // don't save empty document
    if (_document.length() == 0)
    {
        logerr("Failed to save document: document is empty");
        return false;
    }

    // verify the document
    if (_verify != nullptr && !_verify(*this))
    {
        logerr("Failed to save document: validation failed");
        return false;
    }

    // verify the schema
    if (!_schema.verifyDocument(_document))
    {
        logerr("Failed to save document: schema verification failed");
        return false;
    }

    // save document with the ArduinoMongoDB interface
    // get the document _id from the document or create one if it doesn't exist
    String _id = get("_id");
    if (_id.length() == 0)
    {
        _id = nextID();
        set("_id", _id);
    }

    if (!ArduinoMongoDB::createDocument(_document, _collection, _id))
    {
        logerr("Failed to save document: failed to create/update document");
        return false;
    }

    return true;
}

template <typename Callback>
void ArduinoMongoModel::save(Callback callback)
{
    if (!save())
    {
        callback(String(), false);
        return false;
    }

    callback(_document, true);
    return true;
}

// template<typename Callback>
// void ArduinoMongoModel::find_old(bool(*match)(const ArduinoMongoModel&), Callback callback)
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
// void ArduinoMongoModel::find_old(const String &id, Callback callback)
// {
//     ArduinoMongoDB::findDocumentById(_collection, id, [&](bool err, const char* msg, const String& docString){
//         callback(err?msg:"", err?createNew():createNew(docString));
//     });
// }

// const ArduinoMongoModel& ArduinoMongoModel::createNew_old() const
// {
//     return ArduinoMongoModel{_collection, _schema};
// }

// const ArduinoMongoModel& ArduinoMongoModel::createNew_old(const String &docString) const
// {
//     ArduinoMongoModel newDoc = createNew();
//     newDoc.set(docString);
//     return newDoc;
// }

// bool ArduinoMongoModel::set_old(const String &docString)
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
// void ArduinoMongoModel::save_old(Callback callback)
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
// void ArduinoMongoModel::remove_old(Callback callback)
// {
//     if(_doc.containsKey("_id")){ // if the doc is saved
//         ArduinoMongoDB::removeDocument(_collection, _doc["_id"], [&](bool err, const char* msg){
//             callback(err?msg:"", *this);
//         });
//     }
// }
