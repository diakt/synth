#ifndef MXML
#define MXML

#include <libxml/tree.h>
#include <string>
#include <vector>

struct MeasureAttribute {
    int beats;
    int beatType;
    int divisions;
};

struct Chord {
    std::vector<std::pair<int, std::string>> octNotes;
    int duration;
}; 

struct Measure {
    int measurePos;
    MeasureAttribute attributes;
    std::vector<Chord> chords;
};

struct Part {
    std::string partName;
    std::vector<Measure> measures;
};

std::vector<Part> partwiseParser(xmlNode* node, std::vector<Part>& partList);
std::vector<Part> parseXml(const std::string& pfn);

class MxmlParser {
    private:
        std::string inputFile;
        std::vector<Part> parsedMxml;

        //utils
        std::vector<Part> partwiseParser(xmlNode* node, std::vector<Part>& partList);
        Part partParser(xmlNode* partNode);
        Measure measureParser(xmlNode* measureNode);
        std::pair<bool, Chord> chordParser(xmlNode* chordNode);
        MeasureAttribute measureAttributeParser(xmlNode* attribNode);

        //tconv
        int xmlStrContToInt(xmlNode* node);
        std::string xmlStrContToStr(xmlNode* node);
        int xmlStrPropToInt(xmlNode* node, const unsigned char* arg);
        std::string xmlStrPropToStr(xmlNode* node, const unsigned char* arg);

        
    public:
        MxmlParser();
        ~MxmlParser();
        void parseMxml();
        void setInputFile(std::string fn);
        std::vector<Part> getParsedMxml();
};


#endif