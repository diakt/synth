#include "wavefile.hpp"
#include <cstring>
#include <cstdio>
#include <cmath>

bool WriteWaveFile(const char *szFileName, void *pData, int32_t nDataSize, int16_t nNumChannels, int32_t nSampleRate, int32_t nBitsPerSample)
{
    FILE *File = fopen(szFileName, "w+b");
    if(!File)
    {
        return false;
    }

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
    fwrite(pData, nDataSize, 1, File);

    fclose(File);
    return true;
}

std::vector<int16_t> generateSineWave(int sampleRate, int duration, int frequency)
{
    std::vector<int16_t> audioData(sampleRate * duration);
    for (int i = 0; i < sampleRate * duration; ++i) {
        audioData[i] = static_cast<int16_t>(32767 * sin(2 * M_PI * frequency * i / sampleRate));
    }
    return audioData;
}