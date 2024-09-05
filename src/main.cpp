// #include "wavefile.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include "mxml_parser.hpp"
#include "wavefile.hpp"

int main() {

    //Scoreparsing from musicxml
    std::string inputMxml = "chords";
    std::vector<Part> mxml = parseXml(inputMxml);

    //Config for file generation
    std::unordered_map<std::string, int> config {
        {"nSampleRate", 44100},
        {"nNumChannels", 1},
        {"volume", 1},
    
    };

    AudioProcessor audioProcessor;
    audioProcessor.setConfig(config);
    float* audioData = audioProcessor.genFloat(mxml);

    //Gen float array
    // float* audioData = mxmlFactory(mxml, config);


    //Define and write to fn
    std::string outputFile = audioProcessor.genFileName(inputMxml);
    bool success = audioProcessor.WriteWaveFile<int32_t>(
        outputFile.c_str(),
        audioData, 
        static_cast<int32_t>(audioProcessor.config["nNumSamples"]),  //note nNumSamples is added  in mxmlFac
        static_cast<int16_t>(audioProcessor.config["nNumChannels"]), 
        static_cast<int32_t>(audioProcessor.config["nSampleRate"])
    );

    if (!success) {
        std::cout << "Failed to write wave file." << std::endl;
        return 1;
    } 
    return 0;
}