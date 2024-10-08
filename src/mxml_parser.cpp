
#include "mxml_parser.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <string>
#include <vector>

#define USTR(x) (const unsigned char*)(x)

MxmlParser::MxmlParser() {}

MxmlParser::~MxmlParser() {}

//////////////////////////////////////////
////// xml harvest utils /////////////////
//////////////////////////////////////////

int MxmlParser::xmlStrContToInt(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    if (cont) {
        size_t contL = xmlStrlen(cont);
        std::string temp(static_cast<const char*>(static_cast<const void*>(cont)), contL);
        return std::stoi(temp);
    }
    return -43;  // TODO - again magic num
}

std::string MxmlParser::xmlStrContToStr(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    if (cont) {
        size_t contL = xmlStrlen(cont);
        return std::string(static_cast<const char*>(static_cast<const void*>(cont)), contL);
    }
    return "Unknown";
}

int MxmlParser::xmlStrPropToInt(xmlNode* node, const unsigned char* arg) {
    xmlChar* prop = xmlGetProp(node, arg);
    if (prop) {
        size_t propL = xmlStrlen(prop);
        std::string temp(static_cast<const char*>(static_cast<const void*>(prop)), propL);
        return std::stoi(temp);
    }
    return -42;  // TODO - magic num
}

std::string MxmlParser::xmlStrPropToStr(xmlNode* node, const unsigned char* arg) {
    xmlChar* prop = xmlGetProp(node, arg);
    if (prop) {
        size_t propL = xmlStrlen(prop);
        return std::string(static_cast<const char*>(static_cast<const void*>(prop)), propL);
    }
    return "Unknown";
}

std::shared_ptr<xmlNode> makeXmlNodeShared(xmlNode* node) {
    return std::shared_ptr<xmlNode>(node, [](xmlNode*) {});
}

//////////////////////////////////////////
//////////// main res ////////////////////
//////////////////////////////////////////

MeasureAttribute MxmlParser::measureAttributeParser(xmlNode* attribNode) {
    MeasureAttribute thisAttribute;

    if (attribNode) {
        for (auto child = makeXmlNodeShared(attribNode->children); child; child = makeXmlNodeShared(child->next)) {
            if (xmlStrcmp(child.get()->name, attribPhrases[0]) == 0) {  // divisions
                thisAttribute.divisions = xmlStrContToInt(child.get());
            } else if (xmlStrcmp(child.get()->name, attribPhrases[1]) == 0) {  // time

                for (auto subChild = makeXmlNodeShared(child.get()->children); subChild; subChild = makeXmlNodeShared(subChild->next)) {
                    if (xmlStrcmp(subChild->name, attribPhrases[2]) == 0) {
                        thisAttribute.beats = xmlStrContToInt(subChild.get());
                    } else if (xmlStrcmp(subChild->name, attribPhrases[3]) == 0) {
                        thisAttribute.beatType = xmlStrContToInt(subChild.get());
                    }
                }
            }
        }
    }
    return thisAttribute;
}

std::pair<bool, Chord> MxmlParser::chordParser(xmlNode* chordNode) {
    bool isChord;
    Chord thisChord;
    int alter = 0;

    if (chordNode) {
        std::pair<int, std::string> currNote;
        for (auto child = makeXmlNodeShared(chordNode->children); child; child = makeXmlNodeShared(child.get()->next)) {
            if (xmlStrcmp(child.get()->name, chordPhrases[0]) == 0) {  // pitch
                for (auto subChild = makeXmlNodeShared(child.get()->children); subChild; subChild = makeXmlNodeShared(subChild->next)) {
                    if (xmlStrcmp(subChild.get()->name, chordPhrases[1]) == 0) {  // octave
                        currNote.first = xmlStrContToInt(subChild.get());
                    } else if (xmlStrcmp(subChild.get()->name, chordPhrases[2]) == 0) {  // step
                        currNote.second = xmlStrContToStr(subChild.get());
                    } else if (xmlStrcmp(subChild.get()->name, chordPhrases[5]) == 0) {  // alter flag
                        alter = xmlStrContToInt(subChild.get());  // if -1, flat
                    }
                }
            } else if (xmlStrcmp(child.get()->name, chordPhrases[3]) == 0) {  // duration
                thisChord.duration = xmlStrContToInt(child.get());
            } else if (xmlStrcmp(child.get()->name, chordPhrases[4]) == 0) {  // chord flag
                isChord = true;
            }
        }

        // TODO - Handling alters more gracefully
        if (alter != 0) {
            if (alter == -1) {
                currNote.second += "b";
            } else {
                currNote.second += "#";
            }
        }
        thisChord.octNotes.push_back(currNote);
    }
    return std::pair<bool, Chord>(isChord, thisChord);
}

Measure MxmlParser::measureParser(xmlNode* measureNode) {
    Measure thisMeasure;

    if (measureNode) {
        thisMeasure.measurePos = xmlStrPropToInt(measureNode, measurePhrases[0]);  // number
        bool chordFlag = 0;
        std::pair<bool, Chord> ret;

        for (auto child = makeXmlNodeShared(measureNode->children); child; child = makeXmlNodeShared(child->next)) {
            if (xmlStrcmp(child->name, measurePhrases[1]) == 0) {  // attributes
                thisMeasure.attributes = measureAttributeParser(child.get());

            } else if (xmlStrcmp(child.get()->name, measurePhrases[2]) == 0) {  // note
                ret = chordParser(child.get());
                if (ret.first) {       // indicates chord found
                    if (!chordFlag) {  // no current chord, so new elt to chords
                        chordFlag = 1;
                        thisMeasure.chords.push_back(ret.second);
                    } else {  // indicated chord
                        thisMeasure.chords[thisMeasure.chords.size() - 1].octNotes.push_back(ret.second.octNotes[0]);
                    }
                } else {
                    chordFlag = 0;
                    thisMeasure.chords.push_back(ret.second);
                }
            }
        }
    }

    return thisMeasure;
}

Part MxmlParser::partParser(xmlNode* partNode) {
    Part thisPart;

    if (partNode) {
        thisPart.partName = xmlStrPropToStr(partNode, partPhrases[0]);
        for (auto child = std::shared_ptr<xmlNode>(partNode->children, [](xmlNode*) {}); child; child = std::shared_ptr<xmlNode>(child->next, [](xmlNode*) {})) {
            if (child->type != XML_TEXT_NODE) {
                if (xmlStrcmp(child->name, partPhrases[1]) == 0) {
                    thisPart.measures.push_back(measureParser(child.get()));
                }
            }
        }
    }

    return thisPart;
}

std::vector<Part> MxmlParser::partwiseParser(xmlNode* node, std::vector<Part>& partList) {
    std::vector<Part> thisPartwise;
    if (node) {
        for (auto child = std::shared_ptr<xmlNode>(node->children, [](xmlNode*) {}); child; child = std::shared_ptr<xmlNode>(child->next, [](xmlNode*) {})) {
            if (child->type != XML_TEXT_NODE) {  // not just a placeholder
                if (xmlStrcmp(child->name, mainPhrases[0]) == 0) {
                    thisPartwise.push_back(partParser(child.get()));
                }
            }
        }
    }
    return thisPartwise;
}

void MxmlParser::setInputFile(std::string fn) {
    inputFile = "./res/mxml/tests/" + fn + ".musicxml";
}

void MxmlParser::parseMxml() {
    std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)> docPtr(
        xmlReadFile(inputFile.c_str(), nullptr, 0),  // 2part spec file encoding
        xmlFreeDoc);

    if (!docPtr) {
        std::cerr << "Error: could not parse file" << std::endl;
        return;
    }

    xmlNode* root_element = xmlDocGetRootElement(docPtr.get());
    parsedMxml = partwiseParser(root_element, parsedMxml);
    xmlCleanupParser();
}

std::vector<Part> MxmlParser::getParsedMxml() {
    return parsedMxml;
}
