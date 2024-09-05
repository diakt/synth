#ifndef RFXWAVEFILE
#define RFXWAVEFILE

#include <string>
#include <unordered_map>
#include <vector>
#include "mxml_parser.hpp"


struct SMinimalWaveFileHeader {
    unsigned char m_szChunkID[4];
    uint32_t m_nChunkSize;
    unsigned char m_szFormat[4];
    unsigned char m_szSubChunk1ID[4];
    uint32_t m_nSubChunk1Size;
    uint16_t m_nAudioFormat;
    uint16_t m_nNumChannels;
    uint32_t m_nSampleRate;
    uint32_t m_nByteRate;
    uint16_t m_nBlockAlign;
    uint16_t m_nBitsPerSample;
    unsigned char m_szSubChunk2ID[4];
    uint32_t m_nSubChunk2Size;
};

class AudioProcessor {
   private:
        std::unordered_map<std::string, int> keyMap;
        int sampleRate;
        int numChannels;
        float volume;
        

        float getFreq(int octave, int note);

        int maxMeasure(std::vector<Part>& mxml);
        std::unordered_map<int,int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config);
        std::pair<std::string, std::string> inputOutput;
        void convFromFloat(float fIn, int32_t& tOut);

   public:
        AudioProcessor();
        AudioProcessor(std::unordered_map<std::string, int> config);
        ~AudioProcessor();
        void setSampleRate(int newSampleRate);
        void setNumChannels(int newNumChannels);
        void setVolume(int newVolume);
        void setConfig(std::unordered_map<std::string, int> config);
        void setInputOutput(std::string baseName);
        float* genFloat(std::vector<Part>& mxml);
        std::string genFileName(std::string& fn);
        std::unordered_map<std::string, int> config;
        template <typename T>
        bool WriteWaveFile(const char* szFileName, float* floatData, int32_t nNumSamples, int16_t nNumChannels, int32_t nSampleRate);
};



template <typename T>
bool WriteWaveFile(const char* szFileName, float* floatData, int32_t nNumSamples, int16_t nNumChannels, int32_t nSampleRate) {
    FILE* File = fopen(szFileName, "w+b");
    if (!File) {
        return false;
    }
    int32_t nBitsPerSample = sizeof(T) * 8;
    int nDataSize = nNumSamples * sizeof(T);

    SMinimalWaveFileHeader waveHeader;

    memcpy(waveHeader.m_szChunkID, "RIFF", 4);
    waveHeader.m_nChunkSize = nDataSize + 36;
    memcpy(waveHeader.m_szFormat, "WAVE", 4);

    memcpy(waveHeader.m_szSubChunk1ID, "fmt ", 4);
    waveHeader.m_nSubChunk1Size = 16;
    waveHeader.m_nAudioFormat = 1;
    waveHeader.m_nNumChannels = nNumChannels;
    waveHeader.m_nSampleRate = nSampleRate;
    waveHeader.m_nByteRate = nSampleRate * nNumChannels * nBitsPerSample / 8;
    waveHeader.m_nBlockAlign = nNumChannels * nBitsPerSample / 8;
    waveHeader.m_nBitsPerSample = nBitsPerSample;

    memcpy(waveHeader.m_szSubChunk2ID, "data", 4);
    waveHeader.m_nSubChunk2Size = nDataSize;

    fwrite(&waveHeader, sizeof(SMinimalWaveFileHeader), 1, File);

    // update to write
    T* pData = new T[nNumSamples];
    for (int i = 0; i < nNumSamples; ++i) {
        convFromFloat(floatData[i], pData[i]);
    }

    fwrite(pData, nDataSize, 1, File);
    delete[] pData;

    fclose(File);
    return true;
}


#endif