#include "XMLReader.h"
#include "DataSource.h"
#include "DataSink.h"
#include <expat.h>
#include <iostream>

struct CXMLReader::SImplementation { 
    std::shared_ptr<CDataSource> source;    // shared ptr to data source
    XML_Parser parser;  //expat parser
    bool parseEnd = false;  // at end of parsing
    bool EntityReady = false; // Indicates if an entity is ready to be processed
    SXMLEntity CurrentEntity;

    // Constructor: Initialize the parser and set up callbacks
    SImplementation(std::shared_ptr<CDataSource> src): source(src){
        parser = XML_ParserCreate(nullptr);  // create expat parser
        XML_SetUserData(parser, this);  // set user data for callbacks
        XML_SetElementHandler(parser, StartEHand, EndEHand);    // set start and end handlers
        XML_SetCharacterDataHandler(parser, CharDataHand);  // set char data handler
    }

    // Destructor 
    ~SImplementation() {
        XML_ParserFree(parser);
    }
    // start element callback
    static void StartEHand(void *userData, const char *name, const char **attrs) {
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        SXMLEntity entity;  // entity for parsed data
        entity.DType = SXMLEntity::EType::StartElement; // set entity type to start element
        entity.DNameData = name;

        int attributeCount = 0; //tracker
        while(*attrs){  //add atributes to entity
            std::string attrName = *attrs++;    
            std::string attrValue = *attrs++;
            entity.SetAttribute(attrName, attrValue);   
            *attrs++;
        }
        std::cout << "StartElement: " << name << std::endl;    
        std::cout << "No of Attributes: " << attributeCount << std::endl;

        implementation->CurrentEntity =entity;
        implementation->EntityReady = true;
    }
    // end element callback
    static void EndEHand(void *userData, const char *name){
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        std::cout << "EndElement: " << name << std::endl;
        std::cout << "No of Attributes: 0" << std::endl;
        implementation->CurrentEntity = entity;
        implementation->EntityReady = true; 
    }
    // char data calback
    static void CharDataHand(void *userData, const char *data, int len) {                
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        if (len > 0) {
            SXMLEntity entity;
            std::cout << "CharData: " << std::string(data, len) << std::endl;
            entity.DType = SXMLEntity::EType::CharData;
            entity.DNameData = std::string(data, len);
            implementation->CurrentEntity = entity;
            implementation->EntityReady = true; 
        }
    }
    void entityCallback(const SXMLEntity &entity) {
        std::cout << "Entity: " << entity.DNameData << std::endl;
    }
};

// Constructor for CXMLReader class
CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src) 
: implementation(std::make_unique<SImplementation>(src)) {}

// Destructor
CXMLReader::~CXMLReader() = default;

// End of parsing check
bool CXMLReader::End() const {
    return implementation->parseEnd;
}

// Read entities
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    char buffer[1024];
    bool done = false;
    
    while (!done) {
        std::vector<char> buf;
        if (implementation->source->Read(buf, sizeof(buffer))) {
            int len = buf.size();
            done = len == 0;

            if (!done) {
                if (XML_Parse(implementation->parser, buf.data(), len, done) == XML_STATUS_ERROR) {
                    return false;
                }
            }
        }
        else{
            done = true;
        }
        if (implementation->EntityReady) { // Check if an entity is ready
            entity = implementation->CurrentEntity;
            implementation->EntityReady = false; // Reset the flag
            return true;
        }
    }
    implementation->parseEnd = done; // Add this line to mark the end of parsing
    return false;
}