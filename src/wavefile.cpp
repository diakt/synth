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

std::vector<int16_t> generateSineWave(int sampleRate, int duration, int frequency, int split)
{
    // sampleRate is simply sounds per second
    // duration is total duration of sound
    // frequency determines pitch, so A4=440

    std::vector<int16_t> audioData(sampleRate * duration);
    int base = frequency;
    int ssr = split*sampleRate;
    if (split!=1){
        for (int i = 0; i < sampleRate * duration; ++i) {
            if (i%sampleRate==0){
                if (i%(ssr)==0){ 
                    frequency=base;
                } else {
                    frequency*=2;
                }
            }
            audioData[i] = static_cast<int16_t>(32767 * sin(2 * M_PI * frequency * i/sampleRate));
        }
    } else {
        for (int i = 0; i < sampleRate * duration; ++i) {
            audioData[i] = static_cast<int16_t>(32767 * sin(2 * M_PI * frequency * i / sampleRate));
        }
    }
    
    return audioData;
}

std::vector<int32_t> generateSawWave(int sampleRate, int duration)
{
    // TODO - Doubling length of expected saw wave sample due to type mismatch
    int nNumSamples = sampleRate*duration*1; //1 for channels
    std::vector<int32_t> audioData(nNumSamples);
    int32_t nValue = 0;
    for(int nIndex = 0; nIndex < nNumSamples; ++nIndex)
    {
        nValue += 8000000;
        audioData[nIndex] = nValue;
    }
    
    
    return audioData;
}

