#include "wavefile.hpp"
#include <iostream>
#include <chrono>

int main() {

    char* fn = getFileName();

    std::cout << "Generating WAV file at location" << fn << std::endl;
    
    int nSampleRate = 44100;
    int nNumSeconds = 10;  
    int nNumChannels = 1; 

    int32_t *sawData = generateSawWave(nSampleRate, nNumSeconds, nNumChannels);


    bool success = WriteWaveFile(
        fn,
        sawData, 
        static_cast<int32_t>(nSampleRate*nNumSeconds*nNumChannels * sizeof(int32_t)), 
        static_cast<int16_t>(1),  //cardchannels
        static_cast<int32_t>(44100), //freq
        static_cast<int32_t>(32)    //
        );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}