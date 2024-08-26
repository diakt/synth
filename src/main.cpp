#include "wavefile.hpp"
#include "mxml_parser.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>

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

    //Gen float array
    float* audioData = mxmlFactory(mxml, config);

    //Define and write to fn
    char* outputFile = getFileName(inputMxml);
    bool success = WriteWaveFile<int32_t>(
        outputFile,
        audioData, 
        static_cast<int32_t>(config["nNumSamples"]),  //note nNumSamples is added  in mxmlFac
        static_cast<int16_t>(config["nNumChannels"]), 
        static_cast<int32_t>(config["nSampleRate"])
    );

    if (!success) {
        std::cout << "Failed to write wave file." << std::endl;
        return 1;
    } 
    return 0;
}