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

#include "mxml_parser.hpp"

// ENUM

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

// FILE UTILS

char* getFileName(std::string& fn) {
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);

    std::string str = std::to_string(now_c);
    char* char_arr = new char[str.length() + 1];
    std::strcpy(char_arr, str.c_str());

    const char* base = "output/";
    const char* sigh = "_";
    const char* end = ".wav";

    char* res = new char[strlen(base) + strlen(fn.c_str()) + strlen(sigh) + strlen(char_arr) + strlen(end)];
    std::strcpy(res, base);
    std::strcat(res, fn.c_str());
    std::strcat(res, sigh);
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
void convFromFloat(float fIn, uint8_t& tOut) {
    fIn = (fIn + 1.0f) * 127.5f;
    fIn = std::min(255.0f, std::max(0.0f, fIn));
    tOut = static_cast<uint8_t>(fIn);
}

// Overload for int16_t
void convFromFloat(float fIn, int16_t& tOut) {
    fIn *= 32767.0f;
    fIn = std::min(32767.0f, std::max(-32768.0f, fIn));
    tOut = static_cast<int16_t>(fIn);
}

// Overload for int32_t
void convFromFloat(float fIn, int32_t& tOut) {
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
float advanceOscillator_Sine(float& fPhase, float fFrequency, float fSampleRate) {
    fPhase += 2 * (float)M_PI * fFrequency / (float)fSampleRate;

    while (fPhase >= 2 * (float)M_PI) {
        fPhase -= 2 * (float)M_PI;
    }
    while (fPhase < 0) {
        fPhase += 2 * (float)M_PI;
    }
    return sin(fPhase);
}

float advanceOscillator_Square(float& fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;

    return (fPhase <= 0.5f) ? -1.0f : 1.0f;
}

float advanceOscillator_Saw(float& fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;
    return (fPhase * 2.0f) - 1.0f;
}

float advanceOscillator_Triangle(float& fPhase, float fFrequency, float fSampleRate) {
    fPhase += fFrequency / fSampleRate;

    while (fPhase > 1.0f)
        fPhase -= 1.0f;
    while (fPhase < 0.0f)
        fPhase += 1.0f;

    float fRet = (fPhase <= 0.5f) ? fPhase * 2 : (fRet * 2.0f) - 1.0f;

    return (fPhase * 2.0f) - 1.0f;
}

float advanceOscillator_Noise(float& fPhase, float fFrequency, float fSampleRate,
                              float fLastValue) {
    // last arg dud to preserve
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

int32_t* generateStereoSawWave(int nSampleRate, int nNumSeconds, int nNumChannels) {
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

float* generateSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, float vol) {
    // vol is problem child to the extremes, but res by shift to hpp

    int nNumSamples = nSampleRate * nNumSeconds * nNumChannels;
    float* audioData = new float[nNumSamples];

    float testFreq1 = getFreq(3, 0);
    float testFreq2 = getFreq(3, 3);
    int transfer = nNumSamples / 2;

    float fPhase = 0;
    float lastVal = 0.0f;
    for (int i = 0; i < nNumSamples; ++i) {
        if (i < transfer) {
            audioData[i] = vol * advanceOscillator_Sine(fPhase, testFreq1, nSampleRate);
        } else {
            audioData[i] = vol * advanceOscillator_Sine(fPhase, testFreq2, nSampleRate);
        }
        lastVal = audioData[i];
    }
    return audioData;
}

float* generateMultiSineWave(int nSampleRate, int nNumSeconds, int nNumChannels, int nNotes, float vol) {
    int nNumSamples = nSampleRate * nNumSeconds * nNumChannels;
    float* audioData = new float[nNumSamples];

    std::vector<float> frequencies = {
        getFreq(3, 0),
        getFreq(3, 3),
        getFreq(3, 7),
        getFreq(3, 10)};

    float t;
    float norm = 1.0f / nNotes;

    for (int i = 0; i < nNumSamples; ++i) {
        t = static_cast<float>(i) / nSampleRate;
        float sample = 0;
        for (float x : frequencies) {
            audioData[i] += norm * std::sin(2 * M_PI * x * t);
        }
    }
    return audioData;
}

int maxMeasure(std::vector<Part>& mxml) {
    // what is the highest measure? e.g. 64 bars
    int maxM = 0;
    for (Part& x : mxml) {
        for (Measure& y : x.measures) {  // could just take -1
            // std::cout << "maxMeasure measurePos val " << y.measurePos << std::endl;
            maxM = std::max(maxM, y.measurePos);
        }
    }
    return maxM;
}

std::unordered_map<int, int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config) {
    // calculate relevant weightings of notes (more means cut amplitude to avoid clipping)
    std::unordered_map<int, int> partWeight{{0, 0}};
    int currMeasurePos;
    int baseNoteLength;
    int measurePlace;
    int chordStart, chordEnd, chordCard;
    for (Part& currPart : mxml) {
        std::cout << "Partname: " << currPart.partName << std::endl;
        for (Measure& currMeasure : currPart.measures) {
            currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];                                     // mxml is 1-indexed
            baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;  // if 4, on
            measurePlace = currMeasurePos;
            for (Chord& currChord : currMeasure.chords) {
                chordStart = measurePlace;                                      // this does it
                chordEnd = chordStart + (currChord.duration * baseNoteLength);  // how far to step
                chordCard = currChord.octNotes.size();                          // number notes in chord
                // std::cout << "d cS cE chordCard" << currChord.duration << " " << chordStart << " " << chordEnd <<  " " << chordCard << std::endl;

                // TODO - Necessary with current sequential part parsing approach, change to skips on rfx
                for (int i = chordStart; i < chordEnd; i++) {
                    if (partWeight.find(i) == partWeight.end()) {
                        partWeight[i] = 0;
                    }
                    partWeight[i] += chordCard;
                }
                measurePlace = chordEnd;
            }
        }
    }
    std::cout << "end partWeight \n\n" << std::endl;
    return partWeight;
}

float* mxmlFactory(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config) {
    // additional params
    int measureCount = maxMeasure(mxml);
    std::unordered_map<int, int> partWeight = getWeights(mxml, config);

    int nDataL = measureCount * config["nSampleRate"] * config["nNumChannels"];
    float* audioData = new float[nDataL];
    config["nNumSamples"] = nDataL;

    // add audio
    int currMeasurePos;
    int baseNoteLength;
    int currNote = 0;

    // debugging
    float lastval = 1.0f;

    for (Part& currPart : mxml) {
        // std::cout << "mxmlFactory-currPart: " << currPart.partName << std::endl;
        for (Measure& currMeasure : currPart.measures) {
            // std::cout << "mxmlFactory-currMeasure: " << currMeasure.measurePos << std::endl;
            currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];                                     // mxml is 1-indexed
            baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;  // if 4, on

            int measurePlace = currMeasurePos;
            for (Chord& currChord : currMeasure.chords) {
                int chordStart = measurePlace;
                int chordEnd = measurePlace + (currChord.duration * baseNoteLength);  // how far to step

                // get fFreq
                // TODO - should be done earlier and easier in parser
                std::vector<float> chordFreq{};
                for (std::pair<int, std::string>& currNote : currChord.octNotes) {
                    chordFreq.push_back(getFreq(currNote.first, keyMap[currNote.second]));
                }

                int crossfade = std::min(100, (chordEnd - chordStart) / 2);
                for (int i = chordStart; i <= chordEnd; ++i) {
                    float t = static_cast<float>(i) / config["nSampleRate"];
                    float norm = 1.0f / partWeight[i];
                    for (float& currNote : chordFreq) {
                        audioData[i] += norm * std::sin(2 * M_PI * currNote * t);  // Still clips but passable
                    }
                }
                measurePlace = chordEnd;
            }
        }
    }

    return audioData;
}

class AudioProcessor {
   private:
    std::unordered_map<std::string, int> keyMap;
    int sampleRate;
    int numChannels;
    float volume;

    float getFreq() { return 3.0f; }

   public:
    AudioProcessor();
    ~AudioProcessor();
    void setSampleRate(int newSampleRate);
    void setNumChannels(int newNumChannels);
    void setVolume(int newVolume);
};

AudioProcessor::AudioProcessor() {
    keyMap = {
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
}

AudioProcessor::~AudioProcessor(){};

void AudioProcessor::setSampleRate(int newSampleRate) {
    sampleRate = newSampleRate;
}

void AudioProcessor::setNumChannels(int newNumChannels) {
    numChannels = newNumChannels;
}

void AudioProcessor::setVolume(int newVolume) {
    volume = newVolume;
}