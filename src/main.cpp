// #include "wavefile.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include "mxml_parser.hpp"
#include "wavefile.hpp"

int main() {

    //Config 
    std::string inputFilename = "chords";

    //Config for file generation
    std::unordered_map<std::string, int> outputConfig {
        {"nSampleRate", 44100},
        {"nNumChannels", 1},
        {"volume", 1},
    };


    //Parser object
    MxmlParser mxmlParser;
    mxmlParser.setInputFile(inputFilename);
    mxmlParser.parseMxml();
    std::vector<Part> mxml = mxmlParser.getParsedMxml();

    //Waveform generation and fwrite
    AudioProcessor audioProcessor;
    audioProcessor.setInput(inputFilename);
    audioProcessor.setConfig(outputConfig);
    audioProcessor.genWaveform(mxml);

    bool success = audioProcessor.writeWaveFile();
    if (!success) {
        std::cout << "Failed to write wave file." << std::endl;
        return 1;
    } 
    return 0;
}