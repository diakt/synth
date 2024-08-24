#include "wavefile.hpp"
#include "mxml_parser.hpp"
#include <iostream>
#include <chrono>
#include <string>

int main() {

    //INTAKE OF MXML FILE
    std::string mfn = "helloworld.musicxml";
    std::vector<std::string> mxml = parseXml(mfn);

    for(std::string x: mxml) std::cout << x << " ";
    std::cout << std::endl;
    // return 0;


    int nSampleRate = 44100;
    int nNumSeconds = (mxml.size()-4)/4;
    int nNumChannels = 2; 
    float vol = 1.0f;


    char* fn = getFileName();
    // int32_t *audioData = generateSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels should be 1
    // int32_t *audioData = generateStereoSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels must be 2
    // float *audioData = generateMultiSineWave(nSampleRate, nNumSeconds, nNumChannels, 4, vol); 
    std::pair<int, float*> stuff = mxmlFactory(mxml, nSampleRate, nNumChannels);
    int nNumSamples = stuff.first;

    bool success = WriteWaveFile<int32_t>(
        fn,
        stuff.second, 
        static_cast<int32_t>(nNumSamples), 
        static_cast<int16_t>(nNumChannels), 
        static_cast<int32_t>(nSampleRate)
    );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}