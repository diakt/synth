#ifndef MXML
#define MXML

#include <libxml/tree.h>
#include <string>
#include <vector>

#define USTR(x) (const unsigned char*)(x)

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


class MxmlParser {
    private:
        std::string inputFile;
        std::vector<Part> parsedMxml;

        //phrases
        const std::array<const unsigned char*, 4> attribPhrases =  {
            USTR("divisions"),
            USTR("time"),
            USTR("beats"),
            USTR("beat-type")};
        const std::array<const unsigned char*, 3> measurePhrases = {
            USTR("number"),
            USTR("attributes"),
            USTR("note"),
        };
        const std::array<const unsigned char*, 2> partPhrases = {
            USTR("id"),
            USTR("measure"),
        };
        const std::array<const unsigned char*, 1> mainPhrases = {
            USTR("part"),
        };


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