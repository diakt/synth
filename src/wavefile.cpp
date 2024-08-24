#include "wavefile.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

//ENUM

std::unordered_map<std::string, int> keyMap = {
    {"A", 0},
    {"B", 2},
    {"C", 3},
    {"D", 5},
    {"E", 7},
    {"F", 8},
    {"G", 10}
};


// FILE UTILS

char *getFileName() {
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);

    std::cout << "Current Unix: " << now_c << std::endl;

    std::string str = std::to_string(now_c);
    char *char_arr = new char[str.length() + 1];
    std::strcpy(char_arr, str.c_str());

    const char *base = "output/output_";
    const char *end = ".wav";

    char *res = new char[strlen(base) + strlen(char_arr) + strlen(end)];
    std::strcpy(res, base);
    std::strcat(res, char_arr);
    std::strcat(res, end);

    delete[] char_arr;

    return res;
}

// CLAMP MACRO
#define CLAMP(value, min, max)    \
    {                             \
        if (value < min) {        \
            value = min;          \
        } else if (value > max) { \
            value = max;          \
        }                         \
    };

// TYPE UTILS

// Overload for uint8_t
void convFromFloat(float fIn, uint8_t &tOut) {
    fIn = (fIn + 1.0f) * 127.5f;
    fIn = std::min(255.0f, std::max(0.0f, fIn));
    tOut = static_cast<uint8_t>(fIn);
}

// Overload for int16_t
void convFromFloat(float fIn, int16_t &tOut) {
    fIn *= 32767.0f;
    fIn = std::min(32767.0f, std::max(-32768.0f, fIn));
    tOut = static_cast<int16_t>(fIn);
}

// Overload for int32_t
void convFromFloat(float fIn, int32_t &tOut) {
    double dIn = static_cast<double>(fIn) * 2147483647.0;
    dIn = std::min(2147483647.0, std::max(-2147483648.0, dIn));
    tOut = static_cast<int32_t>(dIn);
}

// SOUND UTILS
float getFreq(int octave, int note) {
    // base is 4 on 0
    return (float)(440 * pow(2.0, ((double)((octave - 4) * 12 + note)) / 12.0));
}

// OSCILLATORS
float advanceOscillator_Sine(float &fPhase, float fFrequency, float fSampleRate) {
    fPhase += 2 * (float)M_PI * fFrequency / (float)fSampleRate;

    while (fPhase >= 2 * (float)M_PI) {
        fPhase -= 2 * (float)M_PI;
    }
    while (fPhase < 0) {
        fPhase += 2 * (float)M_PI;
    }
    return sin(fPhase);
}

float advanceOscillator_Square(float &fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;

    return (fPhase <= 0.5f) ? -1.0f : 1.0f;
}

float advanceOscillator_Saw(float &fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;
    return (fPhase * 2.0f) - 1.0f;
}

float advanceOscillator_Triangle(float &fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;

    float fRet = (fPhase <= 0.5f) ? fPhase * 2 : (fRet * 2.0f) - 1.0f;

    return (fPhase * 2.0f) - 1.0f;
}

float advanceOscillator_Noise(float &fPhase, float fFrequency, float fSampleRate,
                              float fLastValue) {

    //last arg dud to preserve
    unsigned int nLastSeed = (unsigned int)fPhase;
    fPhase += fFrequency / fSampleRate;
    unsigned int nSeed = (unsigned int)fPhase;

    while (fPhase > 2.0f) {
        fPhase -= 1.0f;
    }
    if (nSeed != nLastSeed) {
        float fValue = ((float)rand()) / ((float)RAND_MAX);
        fValue = (fValue * 2.0f) - 1.0f;
        fValue = (fValue < 0) ? -1.0f : 1.0f;  // inc intensity
        return fValue;
    } else {
        return fLastValue;
    }
}

// GENERATE WAVEFORMS

int32_t *generateSawWave(int nSampleRate, int nNumSeconds, int nNumChannels) {
    // TODO - Doubling length of expected saw wave sample due to type mismatch
    int nNumSamples = nSampleRate * nNumChannels * nNumSeconds;
    int32_t *audioData = new int32_t[nNumSamples];

    int32_t nValue = 0;
    for (int nIndex = 0; nIndex < nNumSamples; ++nIndex) {
        nValue += 8000000;
        audioData[nIndex] = nValue;
    }

    return audioData;
}

int32_t *generateStereoSawWave(int nSampleRate, int nNumSeconds, int nNumChannels) {
    // TODO - Doubling length of expected saw wave sample due to type mismatch
    int nNumSamples = nSampleRate * nNumChannels * nNumSeconds;
    int32_t *audioData = new int32_t[nNumSamples];

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

float *generateSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, float vol) {
    // vol is problem child to the extremes, but res by shift to hpp

    int nNumSamples = nSampleRate * nNumSeconds * nNumChannels;
    float *audioData = new float[nNumSamples];

    float testFreq1 = getFreq(3, 0);
    float testFreq2 = getFreq(3, 3);
    int transfer = nNumSamples / 2;

    float fPhase = 0;
    float lastVal = 0.0f;
    for (int i = 0; i < nNumSamples; ++i) {
        if (i<transfer){
            audioData[i] = vol * advanceOscillator_Sine(fPhase, testFreq1, nSampleRate);
        } else {
            audioData[i] = vol * advanceOscillator_Sine(fPhase, testFreq2, nSampleRate);
        }
        lastVal = audioData[i]; 
    }
    return audioData;
}

float* generateMultiSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, int nNotes, float vol){
    int nNumSamples = nSampleRate * nNumSeconds * nNumChannels;
    float *audioData = new float[nNumSamples];

    std::vector<float> frequencies = {
        getFreq(3,0),
        getFreq(3,3),
        getFreq(3,7),
        getFreq(3, 10)
    };

    float t;
    float norm = 1.0f/nNotes;

    for(int i=0; i < nNumSamples; ++i){
        t = static_cast<float>(i)/nSampleRate;
        float sample = 0;
        for (float x: frequencies){
            audioData[i]+=norm*std::sin(2*M_PI*x*t);
        }


    }
    return audioData;
}


int process(std::vector<std::string>&mxml, std::vector<std::pair<int, float>>& frequencies){
    int pos = 0;
    for(int i=0; i < mxml.size(); i++){
        if (mxml[i]=="NOTE"){
            std::pair<int, float> curr = {1, 1.0f};
            curr.first = std::stoi(mxml[i+3]);
            int oct = std::stoi(mxml[i+1]);
            int note = keyMap[mxml[i+2]];
            curr.second = getFreq(oct, note);
            pos+=curr.first;
            
            frequencies.push_back(curr);
        }
    }
    return pos;
}


std::pair<int, float*> mxmlFactory(std::vector<std::string>& mxml, int nSampleRate, int nNumChannels){

    //generate part notes
    std::vector<std::pair<int, float>> frequencies = {};
    int nNotes = process(mxml, frequencies);
    int nBaseNoteLength = nSampleRate/4; //should later be connected to measure attrib
    std::cout << "nNotes nBaseNoteLength nNumChannels " << nNotes << " " << nBaseNoteLength << " " << nNumChannels << std::endl;

    int nNumSamples = nNotes * nBaseNoteLength * nNumChannels;
    float *audioData = new float[nNumSamples];

    float t;
    float norm = 1.0f; // /frequencies.size() previously to account for multiple note amp weighting. Eventually will be some sort of tracking current notes run on all players
    int p = 0, ap = 0, endp = 0;
    int fl = frequencies.size();
    std::pair<int, float> curr;
    float fPhase = 0;

    while (p < fl){
        curr = frequencies[p];
        endp = ap+curr.first*nBaseNoteLength*nNumChannels;
        for (int i=ap; i < endp; i++){
            audioData[i]+=advanceOscillator_Sine(fPhase, curr.second, nSampleRate); //avoiding clipping
        }
        p+=1;
        ap = endp;
    }
    return std::pair<int, float*>{nNumSamples, audioData};
}
