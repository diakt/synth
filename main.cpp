#include "wavefile.hpp"
#include <iostream>

int main() {
    std::cout << "Generating WAV file..." << std::endl;
    
    const char* fn = "output.wav";
    int sampleRate = 44100;
    int duration = 100;  // seconds
    int frequency = 440;  // Hz

    std::vector<int16_t> audioData = generateSineWave(sampleRate, duration, frequency);

    void* pData = audioData.data();
    int32_t dataSize = audioData.size() * sizeof(int16_t);
    int16_t numChannels = 1;  // Mono
    int32_t bitsPerSample = 16;

    bool success = WriteWaveFile(fn, pData, dataSize, numChannels, sampleRate, bitsPerSample);

    if (success) {
        std::cout << "Wave file written successfully." << std::endl;
    } else {
        std::cout << "Failed to write wave file." << std::endl;
    }

    return 0;
}