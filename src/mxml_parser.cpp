#include "mxml_parser.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <array>
#include <iostream>
#include <string>
#include <vector>

#define USTR(x) (const unsigned char*)(x)

// TEMP TYPE UTILS
int xmlStrContToInt(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return std::stoi(temp);
}

std::string xmlStrContToStr(xmlNode* node) {
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return temp;
}

int xmlStrPropToInt(xmlNode* node, const unsigned char* arg){
    int res;
    xmlChar* propVal = xmlGetProp(node, arg);
    if (propVal){
        size_t propL= xmlStrlen(propVal);
        std::string temp(reinterpret_cast<const char*>(propVal), propL);
        res = std::stoi(temp);
    } else {
        res = -69;
    }
    return res;
}

std::shared_ptr<xmlNode> makeXmlNodeShared(xmlNode* node) {
    return std::shared_ptr<xmlNode>(node, [](xmlNode*){});
}

//////////////////////////////////////////
//////////// main res ////////////////////
//////////////////////////////////////////

MeasureAttribute measureAttributeParser(xmlNode* attribNode) {
    MeasureAttribute thisAttribute;

    if (attribNode) {
        const unsigned char* attribPhrases[] = {
            USTR("divisions"),
            USTR("time"),
            USTR("beats"),
            USTR("beat-type")};

        for(auto child = makeXmlNodeShared(attribNode->children); child; child = makeXmlNodeShared(child->next)){
            if (xmlStrcmp(child.get()->name, attribPhrases[0]) == 0) {  // divisions
                thisAttribute.divisions = xmlStrContToInt(child.get());
            } else if (xmlStrcmp(child.get()->name, attribPhrases[1]) == 0) {  // time
                xmlNode* temp = child.get()->children;
                while (temp) {
                    if (xmlStrcmp(temp->name, attribPhrases[2]) == 0) {
                        thisAttribute.beats = xmlStrContToInt(temp);
                    } else if (xmlStrcmp(temp->name, attribPhrases[3]) == 0) {
                        thisAttribute.beatType = xmlStrContToInt(temp);
                    }
                    temp = temp->next;
                }
            }
        }
    }
    return thisAttribute;
}

std::pair<bool, Chord> chordParser(xmlNode* chordNode) {
    bool isChord;
    Chord thisChord;
    int alter = 0;

    if (chordNode) {
        const unsigned char* chordPhrases[] = {
            USTR("pitch"),
            USTR("octave"),
            USTR("step"),
            USTR("duration"),
            USTR("chord"),
            USTR("alter")};

        xmlNode* sent = chordNode->children;
        std::pair<int, std::string> currNote;
        for(auto child = std::shared_ptr<xmlNode>(chordNode->children, [](xmlNode*) {}); child; child = std::shared_ptr<xmlNode>(child->next, [](xmlNode*) {})){
            if (xmlStrcmp(child.get()->name, chordPhrases[0]) == 0) {  // pitch
                // std::cout << "pitch found " << std::endl;
                xmlNode* temp = child.get()->children;
                while (temp) {
                    if (xmlStrcmp(temp->name, chordPhrases[1]) == 0) {  // octave
                        currNote.first = xmlStrContToInt(temp);
                    } else if (xmlStrcmp(temp->name, chordPhrases[2]) == 0) {  // step
                        currNote.second = xmlStrContToStr(temp);
                    }
                    temp = temp->next;
                }
            } else if (xmlStrcmp(child.get()->name, chordPhrases[3]) == 0) {  // duration
                thisChord.duration = xmlStrContToInt(child.get());
            } else if (xmlStrcmp(child.get()->name, chordPhrases[4]) == 0) {  // chord flag
                isChord = true;
            } else if (xmlStrcmp(child.get()->name, chordPhrases[5]) == 0) {  // alter flag
                alter = xmlStrContToInt(child.get());                         // if -1, flat
            }
        }

        
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

Measure measureParser(xmlNode* measureNode) {
    Measure thisMeasure;

    if (measureNode) {
        const unsigned char* measurePhrases[] = {
            USTR("number"),
            USTR("attributes"),
            USTR("note"),
        };

        thisMeasure.measurePos = xmlStrPropToInt(measureNode, measurePhrases[0]);

        bool chordFlag = 0;
        std::pair<bool, Chord> ret;

        for(auto child = std::shared_ptr<xmlNode>(measureNode->children, [](xmlNode*) {});child;child = std::shared_ptr<xmlNode>(child->next, [](xmlNode*) {})){
            if (xmlStrcmp(child->name, measurePhrases[1]) == 0) {  // attributes
                thisMeasure.attributes = measureAttributeParser(child.get());

            } else if (xmlStrcmp(child.get()->name, measurePhrases[2]) == 0) {  // note
                ret = chordParser(child.get());
                // std::cout << "chordParser called and prod " << ret.second.octNotes.size() << " " << ret.second.duration << std::endl;

                if (ret.first) {       // indicates chord found
                    if (!chordFlag) {  // no current chord, so new elt to chords
                        chordFlag = 1;
                        thisMeasure.chords.push_back(ret.second);
                    } else {  // indicated chord
                        thisMeasure.chords[thisMeasure.chords.size() - 1].octNotes.push_back(ret.second.octNotes[0]);
                    }

                } else {
                    thisMeasure.chords.push_back(ret.second);
                    chordFlag = 0;
                }
            }
        }
    }
    
    return thisMeasure;
}

Part partParser(xmlNode* partNode) {
    // get the part from the node content. That is the key to the val
    Part thisPart;

    const unsigned char* partPhrases[] = {
        USTR("id"),
        USTR("measure"),
    };

    if (partNode) {
        xmlChar* partId = xmlGetProp(partNode, partPhrases[0]);
        if (partId) {
            size_t propL = xmlStrlen(partId);
            thisPart.partName.assign(reinterpret_cast<const char*>(partId), propL);
        } else {
            thisPart.partName = "Unknown";
        }

        for(auto child = std::shared_ptr<xmlNode>(partNode->children, [](xmlNode*) {}); child; child = std::shared_ptr<xmlNode>(child->next, [](xmlNode*) {})){
            if (child->type != XML_TEXT_NODE) { 
                if (xmlStrcmp(child->name, partPhrases[1]) == 0) {  
                    thisPart.measures.push_back(measureParser(child.get()));
                }
            }
        }
    }

    return thisPart;
}

std::vector<Part> partwiseParser(xmlNode* node, std::vector<Part>& partList) {
    std::vector<Part> thisPartwise;
    if (node) {
        const unsigned char* mainPhrases[] = {
            USTR("part"),
        };

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

std::vector<Part> parseXml(const std::string& pfn) {
    std::vector<Part> res;
    xmlDoc* doc = NULL;
    xmlNode* root_element = NULL;

    std::string mfn = "./res/mxml/tests/" + pfn + ".musicxml";
    const char* fn = mfn.c_str();

    doc = xmlReadFile(fn, NULL, 0);

    if (doc == NULL) {
        std::cout << "Error: could not parse file" << std::endl;
        return res;
    }

    root_element = xmlDocGetRootElement(doc);
    res = partwiseParser(root_element, res);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return res;
}
