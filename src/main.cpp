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
    audioProcessor.genWaveform(mxml);

    bool success = audioProcessor.writeWaveFile();
    if (!success) {
        std::cout << "Failed to write wave file." << std::endl;
        return 1;
    } 
    return 0;
}