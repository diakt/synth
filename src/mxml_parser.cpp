#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <array>
#include <vector>
#include <string>

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

void parseXml(xmlNode* a_node, std::vector<std::string>& res)
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
                parseXml(sent, res);
                sent = sent->next; 
            }
        } 
    }

}










int main() {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile("./res/helloworld.musicxml", NULL, 0);

    if (doc == NULL) {
        std::cout << "Error: could not parse file" << std::endl;
        return -1;
    }
    root_element = xmlDocGetRootElement(doc);
    std::vector<std::string> res;;


    parseXml(root_element, res);

    for(std::string x: res){
        std::cout << x << " "; 
    }
    std::cout << std::endl;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
