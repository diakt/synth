#ifndef RFXWAVEFILE
#define RFXWAVEFILE

#include <string>
#include <unordered_map>
#include <vector>

#include "mxml_parser.hpp"

// TODO - more elegant approach?
namespace std {
    template <>
    struct hash<std::pair<int, int>> {
        size_t operator()(const std::pair<int, int>& p) const {
            return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
        }
    };
}


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
    // var
    inline static std::unordered_map<std::string, int> keyMap = {
        {"A", 0},
        {"A#", 1},
        {"Bb", 1},
        {"B", 2},
        {"C", 3},
        {"C#", 4},
        {"Db", 4},
        {"D", 5},
        {"D#", 6},
        {"Eb", 6},
        {"E", 7},
        {"F", 8},
        {"F#", 9},
        {"Gb", 9},
        {"G", 10},
        {"G#", 11},
        {"Ab", 11},
    };
    std::vector<float> waveform;
    std::unordered_map<std::string, int> config;
    std::string inputFilename;
    std::unordered_map<std::pair<int,int>, float> freqCache;
    

    // const meth
    std::unordered_map<int, int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config) const;
    float getFreq(int octave, int note) const;
    std::string genFileName(std::string& fn) const;
    int maxMeasure(std::vector<Part>& mxml) const;

    void convFromFloat(float fIn, int32_t& tOut);
    // template
    // TODO - rfx this
    template <typename T>
    bool writeWaveFile(int nNumSamples, int nNumChannels, int nSampleRate);

   public:
    AudioProcessor();
    AudioProcessor(std::unordered_map<std::string, int> config);
    ~AudioProcessor();
    void setInput(std::string sInputFilename);
    void setConfig(std::unordered_map<std::string, int> config);
    void genWaveform(std::vector<Part>& mxml);
    bool writeWaveFile();
};

#endif
