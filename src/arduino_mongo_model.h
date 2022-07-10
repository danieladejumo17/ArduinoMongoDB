// // TODO: calculate _schemaBufferSize in Schema Object
// // TODO: Populate fields in models with default values
// // TODO: Add timestamp to schema
// // TODO: Populate collection init file (.mdbc) properly - creation date,
//         // last edit, docs count, and other collection metadata
// // TODO: indexing the database collections by _id and other custom indices for fast lookup

// // TODO: Update schema field to include ObjectId and its ref


// #############################################
// ARDUINO_MONGODB
// TODO: confirm uuid length is not too long for filenames
// FIXME: M,MS,DM,SDMSD,MD,SMDS,DMSDSMD,SMDS,DMSD,SMDSD

// TODO: IDs generated in the ArduinoMomgoDB is only unique on the same machine and not across machines
// TODO: Check the initialization of the collections and database before returning true on exists() methods
// ARDUINO_MONGODB
// #############################################




// // Programming Interface to the DB
// class ArduinoMongoModel{
//     private:
//         const String _collection;
//         DynamicJsonBuffer _jsonBuffer;
//         JsonObject &_doc;
//         const ArduinoMongoSchema &_schema;


//     public:
//         ArduinoMongoModel(const String &collection, const ArduinoMongoSchema &schema)
//             : _collection{collection}, _schema{schema}, _jsonBuffer{schema.bufferSize()}, _doc(_jsonBuffer.createObject())
//         {}

//         ArduinoMongoModel(const ArduinoMongoModel &model)
//             :_collection{model._collection}, _schema{model._schema}, _jsonBuffer{model._schema.bufferSize()}, _doc(_jsonBuffer.createObject())
//         {}

//         template<typename Callback>
//         void find(bool(*)(const ArduinoMongoModel&), Callback);
//         template<typename Callback>
//         void find(const String&, Callback);

//         // void* operator new();
//         // void operator= ();

//         const ArduinoMongoModel& createNew() const;
//         const ArduinoMongoModel& createNew(const String&) const;
//         ArduinoJson::Internals::JsonObjectSubscript<const String &> operator[](const String &key) {return _doc[key];}
//         // operator[](key, type)

//         bool set(const String&);

//         template<typename Callback>
//         void save(Callback);

//         template<typename Callback>
//         void remove(Callback);



// };
