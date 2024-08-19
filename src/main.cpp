#include "wavefile.hpp"
#include <iostream>
#include <chrono>

int main() {

    char* fn = getFileName();

    std::cout << "Generating WAV file at location" << fn << std::endl;
    
    int nSampleRate = 44100;
    int nNumSeconds = 3;  
    int nNumChannels = 2; 

    // int32_t *sawData = generateSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels must be 2
    int32_t *sawData = generateStereoSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels must be 2


    bool success = WriteWaveFile(
        fn,
        sawData, 
        static_cast<int32_t>(nSampleRate*nNumSeconds*nNumChannels * sizeof(int32_t)), 
        static_cast<int16_t>(nNumChannels), 
        static_cast<int32_t>(nSampleRate), 
        static_cast<int32_t>(sizeof(sawData[0])*8)    //
        );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}