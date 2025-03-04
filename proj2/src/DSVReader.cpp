#include "DSVReader.h"
#include "DataSource.h"
#include <vector>
#include <string>
#include <sstream>


struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> source; // Data source to read from 
    char delimiter; // Delimiter character
    // constructor 
    SImplementation(std::shared_ptr<CDataSource> src, char del){
        source = src;
        delimiter = del;
    }

};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char del) {
    implementation = std::make_unique<SImplementation>(src, del);
}

CDSVReader::~CDSVReader() {}

bool CDSVReader::End() const {
    // Implementation for End()
    return implementation->source->End();;
}

bool CDSVReader::ReadRow(std::vector<std::string>& row) {
    if (End()){
        return false;   // end of DSV file
    }
    char ch;
    std::string value; // string of a single value within DSV file
    bool inQuotes = false; // track quoted values
    row.clear();    // clear vector for new row

    while (!implementation->source->End()) {
        implementation->source->Get(ch); // read next character
        if (ch == '\n' && !inQuotes) {   // end oof row and not within quotes
            break;
        }
        if (ch == implementation->delimiter && !inQuotes) {  //delimeter reached & not within quotes
            row.push_back(value);   //add value to row vector
            value.clear();  // clear value for next one
        } 
        else if(ch == '"') { // quotation mark found
            if (inQuotes) { 
                char next;
                if (implementation->source->Peek(next) && next == '"') {
                    value += '"'; // qoute is apart of value
                    implementation->source->Get(ch);  // skip quote
                } 
                else {
                    inQuotes = false; // not within qoutes
                }
            } else {
                inQuotes = true; // going into quotes
            }
        }
        else{
            value += ch;
        }
    }
    if (!value.empty()|| (ch == implementation->delimiter && !inQuotes)) {
        row.push_back(value); // add last value wihout newline to the row
    }

    return true; // Row was successfully read
}