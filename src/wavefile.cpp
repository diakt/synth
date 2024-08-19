#include "wavefile.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <typeinfo>

// UTILS

char* getFileName() {
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);

    std::cout << "Current Unix: " << now_c << std::endl;

    std::string str = std::to_string(now_c);
    char* char_arr = new char[str.length() + 1];
    std::strcpy(char_arr, str.c_str());

    const char* base = "output/output_";
    const char* end = ".wav";

    char* res = new char[strlen(base) + strlen(char_arr) + strlen(end)];
    std::strcpy(res, base);
    std::strcat(res, char_arr);
    std::strcat(res, end);

    delete[] char_arr;

    return res;
}

// WRITE ARRAY TO FILE
bool WriteWaveFile(const char* szFileName, void* pData, int32_t nDataSize,
                   int16_t nNumChannels, int32_t nSampleRate,
                   int32_t nBitsPerSample) {
    FILE* File = fopen(szFileName, "w+b");
    if (!File) {
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

// GENERATE WAVEFORMS
float* generateSineWave(int nSampleRate, int nNumSeconds, int nNumChannels) {
    // sampleRate is simply sounds per second
    // duration is total duration of sound
    // frequency determines pitch, so A4=440
    int nNumSamples = nSampleRate*nNumSeconds*nNumChannels;
    float* audioData = new float[nNumSamples];
    float fFrequency = 440.0;

    for(int i=0; i < nNumSamples; ++i){
        audioData[i] = sin((float)i * 2 * (float)M_PI * fFrequency / (float)nSampleRate);;
    }
    return audioData;
}

int32_t* generateSawWave(int nSampleRate, int nNumSeconds, int nNumChannels) {
    // TODO - Doubling length of expected saw wave sample due to type mismatch
    int nNumSamples = nSampleRate * nNumChannels * nNumSeconds;
    int32_t* audioData = new int32_t[nNumSamples];

    int32_t nValue = 0;
    for (int nIndex = 0; nIndex < nNumSamples; ++nIndex) {
        nValue += 8000000;
        audioData[nIndex] = nValue;
    }

    return audioData;
}

int32_t* generateStereoSawWave(int nSampleRate, int nNumSeconds,
                               int nNumChannels) {
    // TODO - Doubling length of expected saw wave sample due to type mismatch
    int nNumSamples = nSampleRate * nNumChannels * nNumSeconds;
    int32_t* audioData = new int32_t[nNumSamples];

    int32_t nValue1 = 0;
    int32_t nValue2 = 0;
    for (int nIndex = 0; nIndex < nNumSamples; nIndex += 2) {
        nValue1 += 8000000;
        nValue2 += 12000000;
        audioData[nIndex] = nValue1;      // left channel
        audioData[nIndex + 1] = nValue2;  // right channel
    }

    return audioData;
}