#include "wavefile.hpp"
#include <iostream>
#include <chrono>

int main() {

    char* fn = getFileName();

    std::cout << "Generating WAV file at location" << fn << std::endl;
    
    int nSampleRate = 44100;
    int nNumSeconds = 3;  
    int nNumChannels = 1; 
    float vol = 0.00000001;

    // int32_t *audioData = generateSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels should be 1
    // int32_t *audioData = generateStereoSawWave(nSampleRate, nNumSeconds, nNumChannels); //numchannels must be 2
    float *audioData = generateSineWave(nSampleRate, nNumSeconds, nNumChannels, vol); 
    int nNumSamples = nSampleRate*nNumSeconds*nNumChannels;

    bool success = WriteWaveFile(
        fn,
        audioData, 
        static_cast<int32_t>(nSampleRate*nNumSeconds*nNumChannels * sizeof(audioData[0])), 
        static_cast<int16_t>(nNumChannels), 
        static_cast<int32_t>(nSampleRate), 
        static_cast<int32_t>(sizeof(audioData[0])*8)    //
        );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}