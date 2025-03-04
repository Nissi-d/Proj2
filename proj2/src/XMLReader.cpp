#include "XMLReader.h"
#include <expat.h>
#include <iostream>

struct CXMLReader::SImplementation { 
    std::shared_ptr<CDataSource> source;    
    XML_Parser parser;
    SXMLEntity curr;
    bool end; 
    SImplementation(std::shared_ptr<CDataSource> src):source(src), end(false){
        parser = XML_ParserCreate(nullptr); // expat parser
        XML_SetUserData(parser, this);  // set user data for callbacks
        XML_SetElementHandler(parser, StartEHand, EndEHand );
        XML_SetCharacterDataHandler(parser, CharDataHand);  //cha
           
        }
    ~SImplementation() {
        XML_ParserFree(parser);
    }
    static void XMLCALL StartEHand(void *userData, const XML_Char *name, const XML_Char **atts){    //handler for start elements
        auto implementation = static_cast<SImplementation *>(userData); 
        implementation->curr.DType = SXMLEntity::EType::StartElement;   
        implementation->curr.DNameData = name;
        implementation->curr.DAttributes.clear();
        for (int i = 0; atts[i]; i += 2) {  // parse atts
            implementation->curr.DAttributes.emplace_back(atts[i], atts[i + 1]);
        }

    }
    static void XMLCALL EndEHand(void *userData, const XML_Char *name) {  // end elements
        auto implementation = static_cast<SImplementation *>(userData); 
        implementation->curr.DType = SXMLEntity::EType::EndElement;   
        implementation->curr.DNameData = name;
    }
    static void XMLCALL CharDataHand(void *userData, const XML_Char *s, int len){    //handler for start elements
        auto implementation = static_cast<SImplementation *>(userData); 
        implementation->curr.DType = SXMLEntity::EType::CharData;   
        implementation->curr.DNameData.assign(s, len);
    }
    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        char ch;
        while (source->Get(ch)) {
            if (!XML_Parse(parser, &ch, 1, false)) {
                return false;   //
            }
            if (curr.DType != SXMLEntity::EType::CharData || !skipcdata){
                entity = curr;
                return true;
            }
        }
        XML_Parse(parser, nullptr, 0, true);
        end = true;
        return false;
    }
    bool End() const{
        return end;     // whether or not end of data was reached
    }
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src) : implementation(std::make_unique<SImplementation>(src)) {
    // You can initialize your internal state here if needed, using 'src'
}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    // Implement the logic to determine if the XML reading has finished
    return implementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    // Implement logic for reading an entity and potentially skipping cdata if needed
    return implementation->ReadEntity(entity, skipcdata);
}
