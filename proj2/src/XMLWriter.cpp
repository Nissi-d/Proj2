#include "XMLWriter.h"
#include "DataSource.h"
#include "DataSink.h"
#include <iostream>


struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    std::vector<std::string> OpenElements;
    explicit SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(sink) {}
};

// Constructor for XML writer, sink specifies the data destination
CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink) : implementation(std::make_unique<SImplementation>(sink)) {}
    // You can initialize your internal state here if needed, using 'sink'


// Destructor for XML writer
CXMLWriter::~CXMLWriter() {
    // Cleanup code here if needed
}

// Outputs all end elements for those that have been started
bool CXMLWriter::Flush() {
    // Implement the logic to flush the XML content
    for (auto it = implementation->OpenElements.rbegin(); it != implementation->OpenElements.rend(); ++it) {
        std::string endTag = "</" + *it + ">";
        implementation->DataSink->Write(std::vector<char>(endTag.begin(), endTag.end()));
    }
    implementation->OpenElements.clear();
    return true;
}

// Writes out the entity to the output stream
bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    // Implement the logic to write an entity to the data sink
    std::string output;

    switch (entity.DType) {
        case SXMLEntity::EType::StartElement:
            output += "<" + entity.DNameData;
            for (const auto &attr : entity.DAttributes) {
                output += " " + attr.first + "=\"" + attr.second + "\"";
            }
            output += ">";
            implementation->OpenElements.push_back(entity.DNameData);
            break;

        case SXMLEntity::EType::EndElement:
            if (!implementation->OpenElements.empty() && implementation->OpenElements.back() == entity.DNameData) {
                implementation->OpenElements.pop_back();
            }
            output += "</" + entity.DNameData + ">";
            break;

        case SXMLEntity::EType::CharData:
            output += entity.DNameData;
            break;

        case SXMLEntity::EType::CompleteElement:
            output += "<" + entity.DNameData;
            for (const auto &attr : entity.DAttributes) {
                output += " " + attr.first + "=\"" + attr.second + "\"";
            }
            output += "/>";
            break;
    }

    return implementation->DataSink->Write(std::vector<char>(output.begin(), output.end()));
}
