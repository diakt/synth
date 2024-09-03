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
#endif