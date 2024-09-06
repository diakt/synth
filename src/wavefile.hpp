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
    // var
    std::unordered_map<std::string, int> keyMap = {
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
    std::unordered_map<int, int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config);
    std::vector<float> waveform;
    // meth
    float getFreq(int octave, int note);
    int maxMeasure(std::vector<Part>& mxml);
    void convFromFloat(float fIn, int32_t& tOut);
    std::string genFileName(std::string& fn);
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
    std::unordered_map<std::string, int> config;
    std::string inputFilename;
    bool writeWaveFile();

    
};


#endif
