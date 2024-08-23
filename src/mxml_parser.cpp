#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <array>


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



std::array<unsigned char*, 3> noteParser(xmlNode* noteNode){
    //Ret {octave (range), step (key), duration (4 means 4 (((divisions))), or 1 whole note)}

    std::array<unsigned char*, 3> res = {NULL, NULL, NULL};

    unsigned char a[] = "pitch";
    unsigned char b[] = "octave";
    unsigned char c[] = "step";
    unsigned char d[] = "duration";

    xmlNode* sent = noteNode->children;
    while(sent){

        if(sent->name == a){
            xmlNode* temp = sent->children;
            while(temp){
                if(temp->name == b){
                    res[0] = xmlNodeGetContent(temp);
                } else if(temp->name == c){
                    res[1] = xmlNodeGetContent(temp);
                }
                temp = temp->next;
            }

        } else if(sent->name == d){
            res[2] = xmlNodeGetContent(sent);
        }

        sent = sent->next;
    }
    std::cout << "fin np" << std::endl;

    return res;
    
}

void parseXml(xmlNode * a_node, int depth = 0)
{   
    const unsigned char note[] = "note";
    std::array<unsigned char*, 3> temp;
    if (a_node != NULL){

        if (a_node->type==XML_TEXT_NODE) return;

        std::cout << a_node->name << std::endl;
        if(xmlStrcmp(a_node->name, note)==0){
            std::cout << "hit" << std::endl;
            temp = noteParser(a_node);
            std::cout << "postnp" << std::endl;

            // SEGFAULTS
            for(int i=0; i < 3; i++){
                std::cout << temp[i] << std::endl;
            }
            std::cout << std::endl;
        }

        if(a_node->children){
            xmlNode* sent = a_node->children;
            while(sent!=NULL){
                parseXml(sent, 0);
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
    // traverseXml(root_element);
    parseXml(root_element);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
