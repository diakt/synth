#ifndef WAVEFILE_HPP
#define WAVEFILE_HPP

#include <stdint.h>
#include <vector>

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

bool WriteWaveFile(const char *szFileName, void *pData, int32_t nDataSize, 
                   int16_t nNumChannels, int32_t nSampleRate, int32_t nBitsPerSample);

std::vector<int16_t> generateSineWave(int sampleRate, int duration, int frequency, bool split);

#endif // WAVEFILE_HPP