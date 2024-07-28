#include "wavefile.hpp"
#include <iostream>

int main() {
    std::cout << "Generating WAV file..." << std::endl;
    
    const char* fn = "output/output.wav";
    int sampleRate = 44100;
    int duration = 100;  // seconds
    int frequency = 440;  //Determines pitch - check resources
	int split = 3;
    std::vector<int16_t> audioData = generateSineWave(sampleRate, duration, frequency, split);
    std::vector<int32_t> sawData = generateSawWave(sampleRate, duration);

    // void* pData = audioData.data();
    // int32_t dataSize = audioData.size() * sizeof(int16_t);
    // int16_t numChannels = 1;  // Mono
    // int32_t bitsPerSample = 16;

    // bool success = WriteWaveFile(fn, pData, dataSize, numChannels, sampleRate, bitsPerSample);

    void* pData = sawData.data();

    bool success = WriteWaveFile(
        "output/saw.wav", 
        pData, 
        static_cast<int32_t>(sawData.size() * sizeof(int32_t)), 
        static_cast<int16_t>(1), 
        static_cast<int32_t>(44100),
        static_cast<int32_t>(16)
        );

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}