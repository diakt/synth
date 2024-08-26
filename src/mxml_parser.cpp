#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <array>
#include <vector>
#include <string>
#include "mxml_parser.hpp"

#define USTR(x) (const unsigned char*)(x)

//TEMP TYPE UTILS
int xmlStrContToInt(xmlNode* node){
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return std::stoi(temp);
}

std::string xmlStrContToStr(xmlNode* node){
    xmlChar* cont = xmlNodeGetContent(node);
    size_t contL = xmlStrlen(cont);
    std::string temp(reinterpret_cast<const char*>(cont), contL);
    return temp;
}


MeasureAttribute measureAttributeParser(xmlNode* attribNode){
    MeasureAttribute thisAttribute;

    if (attribNode){
        const unsigned char* attribPhrases[] =  {
            USTR("divisions"), 
            USTR("time"), 
            USTR("beats"), 
            USTR("beat-type")
        };

        xmlNode* sent = attribNode->children;
        while(sent){
            if(xmlStrcmp(sent->name, attribPhrases[0])==0){ //divisions
                thisAttribute.divisions = xmlStrContToInt(sent);
            } else if(xmlStrcmp(sent->name, attribPhrases[1])==0){ //time
                xmlNode* temp = sent->children;
                while(temp){
                    if(xmlStrcmp(temp->name, attribPhrases[2])==0){
                        thisAttribute.beats = xmlStrContToInt(temp);
                    } else if(xmlStrcmp(temp->name, attribPhrases[3])==0){
                        thisAttribute.beatType = xmlStrContToInt(temp);
                    }
                    temp = temp->next;
                }
            }  
            sent = sent->next;
        }
    }
    return thisAttribute;
}

std::pair<bool, Chord> chordParser(xmlNode* chordNode){
    bool isChord;
    Chord thisChord;

    if(chordNode){
        const unsigned char* chordPhrases[] =  {
            USTR("pitch"), 
            USTR("octave"), 
            USTR("step"), 
            USTR("duration"),
            USTR("chord"),
        };

        xmlNode* sent = chordNode->children;
        std::pair<int, std::string> currNote;
        while(sent){

            if(xmlStrcmp(sent->name, chordPhrases[0])==0){ //pitch
                //std::cout << "pitch found " << std::endl;
                xmlNode* temp = sent->children;
                while(temp){
                    if(xmlStrcmp(temp->name, chordPhrases[1])==0){ //octave
                        currNote.first = xmlStrContToInt(temp);
                    } else if(xmlStrcmp(temp->name, chordPhrases[2])==0){ //step
                        currNote.second = xmlStrContToStr(temp);
                    }
                    temp = temp->next;
                }
            } else if(xmlStrcmp(sent->name, chordPhrases[3])==0){ //duration
                thisChord.duration = xmlStrContToInt(sent);
            } else if(xmlStrcmp(sent->name, chordPhrases[4])==0){ //chord flag
                isChord = true;
            }
            sent = sent->next;
        }
        thisChord.octNotes.push_back(currNote);
    }
    return std::pair<bool, Chord>(isChord, thisChord);
}

Measure measureParser(xmlNode* measureNode){
    Measure thisMeasure;

    if(measureNode){
        const unsigned char* measurePhrases[] =  {
            USTR("number"),
            USTR("attributes"), 
            USTR("note"),
        };



        xmlChar* measureNumber = xmlGetProp(measureNode, measurePhrases[0]);
        if(measureNumber){
            size_t propL = xmlStrlen(measureNumber);
            std::string temp(reinterpret_cast<const char*>(measureNumber), propL);
            thisMeasure.measurePos = std::stoi(temp);
        } else {
            thisMeasure.measurePos = -69;
        }


        bool chordFlag = 0;
        xmlNode* sent = measureNode->children;
        std::pair<bool, Chord> ret;
        while(sent){
            if(xmlStrcmp(sent->name, measurePhrases[1])==0){    //attributes
                thisMeasure.attributes = measureAttributeParser(sent);

            } else if(xmlStrcmp(sent->name, measurePhrases[2])==0){     //note
                ret = chordParser(sent);
                //std::cout << "chordParser called and prod " << ret.second.octNotes.size() << " " << ret.second.duration << std::endl;
                
                if(ret.first){ //indicates chord found
                    if(!chordFlag){ //no current chord, so new elt to chords
                        chordFlag = 1;
                        thisMeasure.chords.push_back(ret.second);
                    } else { //indicated chord
                        thisMeasure.chords[thisMeasure.chords.size()-1].octNotes.push_back(ret.second.octNotes[0]);
                    }

                } else {
                    thisMeasure.chords.push_back(ret.second);
                    chordFlag = 0;
                }
            }
            sent = sent->next;
        }

        

    }
    //std::cout << "does thisMeasure have any chords... " << thisMeasure.chords.size() << std::endl;
    // for(Chord& ch: thisMeasure.chords){
    //     std::cout << "Chord: " << ch.octNotes[0].first << " " << ch.octNotes[0].second << " " << ch.duration << std::endl;
    // }
    return thisMeasure;
}

Part partParser(xmlNode* partNode){
    // get the part from the node content. That is the key to the val
    Part thisPart;

    const unsigned char* partPhrases[] =  {
        USTR("id"), 
        USTR("measure"),
    };

    if(partNode){
        xmlChar* partId = xmlGetProp(partNode, partPhrases[0]);
        if(partId){
            size_t propL = xmlStrlen(partId);
            thisPart.partName.assign(reinterpret_cast<const char*>(partId), propL);
        } else {
            thisPart.partName = "Unknown";
        }

        xmlNode* child = partNode->children;
        while(child){
            if(child->type!=XML_TEXT_NODE){ //not just a placeholder
                //std::cout << "partParser childname = " << child->name << std::endl;
                if (xmlStrcmp(child->name, partPhrases[1])==0){ //this is a part
                    thisPart.measures.push_back(measureParser(child));
                }
            } 
            child = child->next;
        }
    }

    return thisPart;
}

std::vector<Part> partwiseParser(xmlNode* node, std::vector<Part>& partList){
    std::vector<Part> thisPartwise;
    if(node){
        const unsigned char* mainPhrases[] =  {
            USTR("part"), 
        };

        xmlNode* child = node->children;
        while(child){
            if(child->type!=XML_TEXT_NODE){ //not just a placeholder
                if (xmlStrcmp(child->name, mainPhrases[0])==0){ //this is a part
                    //std::cout << "part detected " << std::endl;
                    thisPartwise.push_back(partParser(child));
                }
            } 
            child = child->next;
        }
    }
    return thisPartwise;
}

std::vector<Part> parseXml(const std::string& pfn){

    std::vector<Part> res;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    std::string mfn = "./res/tests/" + pfn + ".musicxml";
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


