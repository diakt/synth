#include "wavefile.hpp"
#include "mxml_parser.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>

int main() {

    //INTAKE OF MXML FILE
    std::string mfn = "chords.musicxml";
    std::vector<Part> mxml = parseXml(mfn);
    // std::cout << "mxml size: " << mxml.size() << std::endl;
    std::cout << "part measurenum " << mxml[0].measures.size() << std::endl;
    // std::cout << "part first measure chords " << mxml[0].measures[0].chords.size() << std::endl;


    std::unordered_map<std::string, int> config;
    config["nSampleRate"] = 44100;
    config["nNumChannels"] = 1;
    config["volume"] = 1;

    char* fn = getFileName();
    std::pair<int, float*> stuff = mxmlFactory(mxml, config);
    int nNumSamples = stuff.first;

    bool success = WriteWaveFile<int32_t>(
        fn,
        stuff.second, 
        static_cast<int32_t>(config["nNumSamples"]), 
        static_cast<int16_t>(config["nNumChannels"]), 
        static_cast<int32_t>(config["nSampleRate"])
    );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}