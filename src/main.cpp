// #include "wavefile.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include "mxml_parser.hpp"
#include "wavefile.hpp"

int main() {

    //Config for file generation
    std::unordered_map<std::string, std::string> inputConfig {
        {"inputFile", "chords"},
    };

    //Config for file generation
    std::unordered_map<std::string, int> outputConfig {
        {"nSampleRate", 44100},
        {"nNumChannels", 1},
        {"volume", 1},
    };

    //Scoreparsing from musicxml
    std::vector<Part> mxml = parseXml(inputConfig["inputFile"]);

    AudioProcessor audioProcessor;
    audioProcessor.setInput(inputConfig["inputFile"]);
    audioProcessor.setConfig(outputConfig);
    float* audioData = audioProcessor.genFloat(mxml);

    //Gen float array
    // float* audioData = mxmlFactory(mxml, config);


    //Define and write to fn
    bool success = audioProcessor.writeWaveFile<int32_t>(
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