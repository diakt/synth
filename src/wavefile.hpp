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
        std::pair<std::string, std::string> inputOutput;

        float getFreq(int octave, int note);
        int maxMeasure(std::vector<Part>& mxml);
        std::unordered_map<int,int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config);
        void convFromFloat(float fIn, int32_t& tOut);

   public:
        AudioProcessor();
        AudioProcessor(std::unordered_map<std::string, int> config);
        ~AudioProcessor();
        void setConfig(std::unordered_map<std::string, int> config);
        void setInputOutput(std::string baseName);
        float* genFloat(std::vector<Part>& mxml);
        std::string genFileName(std::string& fn);
        std::unordered_map<std::string, int> config;
        template <typename T>
        bool WriteWaveFile(const char* szFileName, float* floatData, int32_t nNumSamples, int16_t nNumChannels, int32_t nSampleRate);
};

#endif
