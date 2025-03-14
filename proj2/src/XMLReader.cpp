#include "XMLReader.h"
#include <expat.h>
#include <iostream>
#include <queue>

struct CXMLReader::SImplementation { 
    std::shared_ptr<CDataSource> source;    // shared ptr to data source
    XML_Parser parser;  // expat parser
    bool parseEnd = false;  // at end of parsing
    std::queue<SXMLEntity> entityQueue; // Queue to store parsed entities

    // Constructor: Initialize the parser and set up callbacks
    SImplementation(std::shared_ptr<CDataSource> src): source(src){
        parser = XML_ParserCreate(nullptr);  // create expat parser
        if (!parser) {
            throw std::runtime_error("Failed to create XML parser");
        }
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

        while(*attrs){  // add attributes to entity
            std::string attrName = *attrs++;    
            std::string attrValue = *attrs++;
            entity.SetAttribute(attrName, attrValue);   
        }
        implementation->entityQueue.push(entity); // Add entity to queue
    }

    // end element callback
    static void EndEHand(void *userData, const char *name){
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        implementation->entityQueue.push(entity); // Add entity to queue
    }

    // character data callback
    static void CharDataHand(void *userData, const char *data, int len) {                
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData = std::string(data, len);
        implementation->entityQueue.push(entity); // Add entity to queue
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
    std::vector<char> buffer(1024); // Buffer to hold data from the source
    size_t bytesRead;

    // Process data until an entity is available or parsing ends
    while (implementation->entityQueue.empty() && !implementation->parseEnd) {
        bytesRead = implementation->source->Read(buffer, buffer.size());
        if (bytesRead == 0) {
            // Finalize parsing at source end
            XML_Parse(implementation->parser, nullptr, 0, true);  
            implementation->parseEnd = true;
            break;
        }

        if (!XML_Parse(implementation->parser, buffer.data(), bytesRead, false)) {
            implementation->parseEnd = true;
            return false; // Parsing error
        }
    }

    // Return the next entity from the queue
    if (!implementation->entityQueue.empty()) {
        entity = implementation->entityQueue.front();
        implementation->entityQueue.pop();

        // Skip character data if requested
        if (skipcdata && entity.DType == SXMLEntity::EType::CharData) {
            return ReadEntity(entity, skipcdata); // Recursively skip character data
        }

        return true;
    }

    return false; // No more entities
}