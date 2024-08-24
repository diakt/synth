#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <array>
#include <vector>
#include <string>
#include "mxml_parser.hpp"

#define USTR(x) (const unsigned char*)(x)

bool hasElementChildren(xmlNode* node){
    xmlNode* child = node->children;
    while(child){
        if(child->type == XML_ELEMENT_NODE){
            return true;
        }
        child = child->next;
    }
    return false;
}



void traverseXml(xmlNode * a_node, int depth = 0)
{   
    xmlKeepBlanksDefault(0);
    if (a_node != NULL){

        if (a_node->type==XML_TEXT_NODE) return;

        std::cout << a_node->name << ": "; //<< a_node->type << std::endl;
        if(a_node->type==XML_ELEMENT_NODE) std::cout << "XML_ELEMENT_NODE";
        else if(a_node->type==XML_ATTRIBUTE_NODE) std::cout << "XML_ATTRIBUTE_NODE";
        else if(a_node->type==XML_TEXT_NODE) std::cout << "XML_TEXT_NODE";
        else std::cout << "XML_IDK_NODE "<< a_node->type;
        std::cout << std::endl;

        //attributes
        if(a_node->properties !=NULL){
            xmlAttr* props = a_node->properties;
            std::cout << "PROPS:" << std::endl;
            while(props!=NULL){
                xmlChar* val = xmlNodeListGetString(a_node->doc, props->children, 1);
                std::cout << props->name << ": " << val<< std::endl;
                xmlFree(val);
                props = props->next;
            }
        }

        if (!hasElementChildren(a_node)){
            //content
            xmlChar* content = xmlNodeGetContent(a_node);
            if(content && xmlStrlen(content) > 0){
                std::cout << "CONTENT: ";
                std::cout << sizeof(content) << " <" << content << "> " << std::endl;
            }
        }
        


        std::cout << std::endl;
        
        
        if(a_node->children){
            xmlNode* sent = a_node->children;
            while(sent!=NULL){
                traverseXml(sent, 0);
                sent = sent->next; 
            }
        } 
        

    }

}

std::array<std::string, 3> attribParser(xmlNode* attribNode){
    std::array<std::string, 3> res = {"", "", ""};
    const unsigned char* attribPhrases[] =  {
        USTR("divisions"), 
        USTR("time"), 
        USTR("beats"), 
        USTR("beat-type")
    };

    xmlNode* sent = attribNode->children;
    while(sent){
        if(xmlStrcmp(sent->name, attribPhrases[0])==0){
            xmlChar* dur = xmlNodeGetContent(sent);
            res[2] = reinterpret_cast<char*>(dur);
        } else if(xmlStrcmp(sent->name, attribPhrases[1])==0){
            xmlNode* temp = sent->children;
            while(temp){
                if(xmlStrcmp(temp->name, attribPhrases[2])==0){
                    xmlChar* oct = xmlNodeGetContent(temp);
                    res[0] = reinterpret_cast<char*>(oct);
                } else if(xmlStrcmp(temp->name, attribPhrases[3])==0){
                    xmlChar* step = xmlNodeGetContent(temp);
                    res[1] = reinterpret_cast<char*>(step);
                }
                temp = temp->next;
            }
        }  

        sent = sent->next;
    }
    return res;
}

std::array<std::string, 3> noteParser(xmlNode* noteNode){
    //Ret {octave (range), step (key), duration (4 means 4 (((divisions))), or 1 whole note)}
    //TODO - Must add chord flag to figure out whether or not to add

    std::array<std::string, 3> res = {"", "", ""};
    const unsigned char* notePhrases[] =  {
        USTR("pitch"), 
        USTR("octave"), 
        USTR("step"), 
        USTR("duration")
    };

    xmlNode* sent = noteNode->children;
    while(sent){

        if(xmlStrcmp(sent->name, notePhrases[0])==0){
            xmlNode* temp = sent->children;
            while(temp){
                if(xmlStrcmp(temp->name, notePhrases[1])==0){
                    xmlChar* oct = xmlNodeGetContent(temp);
                    res[0] = reinterpret_cast<char*>(oct);
                } else if(xmlStrcmp(temp->name, notePhrases[2])==0){
                    xmlChar* step = xmlNodeGetContent(temp);
                    res[1] = reinterpret_cast<char*>(step);
                }
                temp = temp->next;
            }

        } else if(xmlStrcmp(sent->name, notePhrases[3])==0){
            xmlChar* dur = xmlNodeGetContent(sent);
            res[2] = reinterpret_cast<char*>(dur);
        }

        sent = sent->next;
    }
    return res;
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

                // size_t propL = xmlStrlen(measureId);
                // std::string temp = reinterpret_cast<const char*>(measureId, propL);
                // thisMeasure.measurePos = std::stoi(temp);

                
                xmlChar* dur = xmlNodeGetContent(sent);
                size_t contentL = xmlStrlen(dur);
                std::string temp = reinterpret_cast<const char*>(dur, contentL);
                thisAttribute.divisions = std::stoi(temp);

            } else if(xmlStrcmp(sent->name, attribPhrases[1])==0){ //time
                xmlNode* temp = sent->children;
                while(temp){
                    if(xmlStrcmp(temp->name, attribPhrases[2])==0){
                        xmlChar* oct = xmlNodeGetContent(temp);
                        size_t contentL = xmlStrlen(oct);
                        std::string temp = reinterpret_cast<const char*>(oct, contentL);
                        thisAttribute.beats = std::stoi(temp);

                    } else if(xmlStrcmp(temp->name, attribPhrases[3])==0){
                        xmlChar* step = xmlNodeGetContent(temp);
                        size_t contentL = xmlStrlen(step);
                        std::string temp = reinterpret_cast<const char*>(step, contentL);
                        thisAttribute.beatType = std::stoi(temp);
                    }
                    temp = temp->next;
                }
            }  

            sent = sent->next;
        }
    }

    return thisAttribute;
}


Chord chordParser(xmlNode* chord){
    Chord thisChord;

    return thisChord;
}


Measure measureParser(xmlNode* measureNode){
    Measure thisMeasure;

    if(measureNode){
        const unsigned char* measurePhrases[] =  {
            USTR("number"), 
        };

        xmlChar* measureId = xmlGetProp(measureNode, measurePhrases[0]);
        if(measureId){
            size_t propL = xmlStrlen(measureId);
            std::string temp = reinterpret_cast<const char*>(measureId, propL);
            thisMeasure.measurePos = std::stoi(temp);
        } else {
            thisMeasure.measurePos = 0; //not good
        }

        bool chordFlag = 0;

    }
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
                std::cout << "partParser childname = " << child->name << std::endl;
                if (xmlStrcmp(child->name, partPhrases[1])==0){ //this is a part
                    thisPart.measures.push_back(measureParser(child));
                }
            } 
            child = child->next;
        }
    }

    return thisPart;
}

void mxml_parser(xmlNode* node, std::vector<Part>& partList){
    //parse from main node to separate out parts
}


void dfs(xmlNode* a_node, std::vector<std::string>& res)
{   
    const unsigned char* genPhrases[] =  {
        USTR("attributes"), 
        USTR("note"), 
    };


    std::array<std::string, 3> temp;
    if (a_node != NULL){
        if (a_node->type==XML_TEXT_NODE) return;


        if(xmlStrcmp(a_node->name, genPhrases[0])==0){ //ATTRIB CATCH
            temp = attribParser(a_node);
            res.push_back("ATTRIB");
            for(std::string x: temp){
                res.push_back(x);
            }
        } else if(xmlStrcmp(a_node->name, genPhrases[1])==0){ //NOTE CATCH
            temp = noteParser(a_node);
            res.push_back("NOTE");
            for(std::string x: temp){
                res.push_back(x);
            }
        } else if (a_node->children){
            xmlNode* sent = a_node->children;
            while(sent!=NULL){
                dfs(sent, res);
                sent = sent->next; 
            }
        } 
    }

}



std::vector<std::string> parseXml(const std::string& pfn){

    std::vector<std::string> res;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    std::string mfn = "./res/tests/" + pfn;
    const char* fn = mfn.c_str();

    doc = xmlReadFile(fn, NULL, 0);

    if (doc == NULL) {
        std::cout << "Error: could not parse file" << std::endl;
        return res;
    }


    root_element = xmlDocGetRootElement(doc);
    dfs(root_element, res);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return res;
}


