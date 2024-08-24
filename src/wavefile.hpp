#ifndef WAVEFILE_HPP
#define WAVEFILE_HPP

#include <stdint.h>
#include <vector>

char* getFileName();


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

//TYPE UTILS
void convFromFloat(float fIn, uint8_t& tOut);
void convFromFloat(float fIn, int16_t& tOut);
void convFromFloat(float fIn, int32_t& tOut);



// WRITE ARRAY TO FILE
template <typename T>
bool WriteWaveFile(const char* szFileName, float* floatData, int32_t nNumSamples, int16_t nNumChannels, int32_t nSampleRate) {
    FILE* File = fopen(szFileName, "w+b");
    if (!File) {
        return false;
    }
    int32_t nBitsPerSample = sizeof(T)*8;
    int nDataSize = nNumSamples*sizeof(T);


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


    //update to write
    T *pData = new T[nNumSamples];
    for(int i=0; i < nNumSamples; ++i){
        convFromFloat(floatData[i], pData[i]);
    }

    fwrite(pData, nDataSize, 1, File);
    delete[] pData;

    fclose(File);
    return true;
}

int32_t* generateSawWave(int nSampleRate, int nNumSeconds, int nNumChannels);

int32_t* generateStereoSawWave(int nSampleRate, int nNumSeconds, int nNumChannels);

float* generateSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, float vol);

float* generateMultiSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, int nNotes, float vol);

std::pair<int, float*> mxmlFactory(std::vector<std::string>& mxml, int nSampleRate, int nNumChannels);
#endif  // WAVEFILE_HPP