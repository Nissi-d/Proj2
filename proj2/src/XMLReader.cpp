#include "XMLReader.h"
#include <expat.h>
#include <iostream>

struct CXMLReader::SImplementation { 
    std::shared_ptr<CDataSource> source;    
    XML_Parser parser;
    SXMLEntity curr;
    bool parseEnd;  // at end of parsing

    // Constructor: Initialize the parser and set up callbacks
    SImplementation(std::shared_ptr<CDataSource> src): source(src), parseEnd(false) {
        parser = XML_ParserCreate(nullptr);  // create expat parser
        XML_SetUserData(parser, this);  // set user data for callbacks
        XML_SetElementHandler(parser, StartEHand, EndEHand);    // set start and end handlers
        XML_SetCharacterDataHandler(parser, CharDataHand);  // set char data handler
    }

    // Destructor: Free the parser when done
    ~SImplementation() {
        XML_ParserFree(parser);
    }
    // start element callback
    static void StartEHand(void *userData, const char *name, const char **attrs) {
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        implementation->curr.DType = SXMLEntity::EType::StartElement;   // set entity type to StartElement
        implementation->curr.DNameData = name; 
        
        implementation->curr.DAttributes.clear();  // clear attributes

        while(*attrs){
            implementation->curr.DAttributes.push_back(
                std::make_pair(std::string(attrs[0]), std::string(attrs[1]))
            );
            attrs +=2;  //jump to next attribute or key:value pair
        }
    }
    // end element callback
    static void EndEHand(void *userData, const char *name){
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        implementation->curr.DType = SXMLEntity::EType::EndElement;
        implementation->curr.DNameData = name;
    }
    // char data calback
    static void CharDataHand(void *userData, const char *data, int len) {                
        SImplementation *implementation = static_cast<SImplementation*>(userData);
        implementation->curr.DType = SXMLEntity::EType::CharData;
        implementation->curr.DNameData.assign(data, len);
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
    char ch;
    while (implementation->source->Get(ch)) {
        if (!XML_Parse(implementation->parser, &ch, 1, false)) {
            return false;
        }
        // if entity not char data or not skipcdata
        if (implementation->curr.DType != SXMLEntity::EType::CharData || !skipcdata) {
            entity = implementation->curr;      // copy
            implementation->curr = SXMLEntity();  // Reset the current entity
            return true;
        }
    }
    // finalize parsing at source end
    XML_Parse(implementation->parser, nullptr, 0, true);  
    implementation->parseEnd = true;    // parsing finished
    return false;
}
