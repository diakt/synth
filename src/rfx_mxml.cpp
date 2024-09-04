#include "mxml_parser.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <array>
#include <iostream>
#include <string>
#include <vector>

#define USTR(x) (const unsigned char*)(x)

class MxmlParser {
    private:
        std::string inputFile;

        //utils
        int xmlStrContToInt(xmlNode* node);
        int xmlStrPropToInt(xmlNode* node, const unsigned char* arg);
        std::string xmlStrContToStr(xmlNode* node);
        std::string xmlStrPropToStr(xmlNode* node, const unsigned char* arg) ;
        std::shared_ptr<xmlNode> makeXmlNodeShared(xmlNode* node);

    public:
        MxmlParser();
        ~MxmlParser();

        //methods
        std::vector<Part> parseMxml(std::string inputFile);

        //nonsense
        void setInputFile(std::string nInputFile){};
        

};

MxmlParser::MxmlParser(){};
MxmlParser::~MxmlParser(){};

void MxmlParser::setInputFile(std::string nInputFile){
    inputFile = nInputFile;
}

std::vector<Part> MxmlParser::parseMxml(std::string inputFile){
    std::vector<Part> res;

    return res;
}


////////////////////////////////////////////////////////////
///////////////////// UTILS ////////////////////////////////
////////////////////////////////////////////////////////////

int MxmlParser::xmlStrContToInt(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return std::stoi(temp);
}

std::string MxmlParser::xmlStrContToStr(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return temp;
}

int MxmlParser::xmlStrPropToInt(xmlNode* node, const unsigned char* arg) {
    int res;
    xmlChar* propVal = xmlGetProp(node, arg);
    if (propVal) {
        size_t propL = xmlStrlen(propVal);
        std::string temp(reinterpret_cast<const char*>(propVal), propL);
        res = std::stoi(temp);
    } else {
        res = -69;
    }
    return res;
}

std::string MxmlParser::xmlStrPropToStr(xmlNode* node, const unsigned char* arg) {
    std::string res;
    xmlChar* propVal = xmlGetProp(node, arg);
    if (propVal) {
        size_t propL = xmlStrlen(propVal);
        std::string temp(reinterpret_cast<const char*>(propVal), propL);
        return temp;
    }
    return "Unknown";
}

std::shared_ptr<xmlNode> makeXmlNodeShared(xmlNode* node) {
    return std::shared_ptr<xmlNode>(node, [](xmlNode*) {});
}
