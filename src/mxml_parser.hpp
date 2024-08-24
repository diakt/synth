#ifndef MXML
#define MXML

#include <libxml/tree.h>
#include <vector>
#include <string>

struct MeasureAttribute{
    int beats;
    int beatType;
    int divisions;
};



struct Chord {
    std::vector<std::pair<int, std::string>> octNotes;
    std::string duration;
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


std::vector<std::string> parseXml(const std::string& pfn);



#endif