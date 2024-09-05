// #include "wavefile.hpp"

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
#include <sstream>

#include "mxml_parser.hpp"
#include "wavefile.hpp"

//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////

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


AudioProcessor::AudioProcessor(std::unordered_map<std::string, int> config) : config(config) {
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



void AudioProcessor::setConfig(std::unordered_map<std::string, int> newConfig){
    config = newConfig;
}

void AudioProcessor::setInputOutput(std::string input){
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "output/" << input << "_" << now_c << ".wav";

    inputOutput.first = "./res/mxml/tests"+input+".musicxml";
    inputOutput.second = ss.str();
}

float AudioProcessor::getFreq(int octave, int note){
    return (float)(440 * pow(2.0, ((double)((octave - 4) * 12 + note)) / 12.0));
}

std::string AudioProcessor::genFileName(std::string& fn) {
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "output/" << fn << "_" << now_c << ".wav";
    return ss.str();
}

int AudioProcessor::maxMeasure(std::vector<Part>& mxml){
    int maxM = 0;
    for (Part& x : mxml){
        for (Measure& y: x.measures){
            maxM = std::max(maxM, y.measurePos);
        }
    }
    return maxM;
}

std::unordered_map<int,int> AudioProcessor::getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config) {
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
    std::cout << "end partWeight \n\n"<< std::endl;
    return partWeight;
}

float* AudioProcessor::genFloat(std::vector<Part>& mxml){
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
        for (Measure& currMeasure : currPart.measures) {
            currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];                                     // mxml is 1-indexed
            baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;  // if 4, on

            int measurePlace = currMeasurePos;
            for (Chord& currChord : currMeasure.chords) {
                int chordStart = measurePlace;
                int chordEnd = measurePlace + (currChord.duration * baseNoteLength);  // how far to step

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

void AudioProcessor::convFromFloat(float fIn, int32_t& tOut){
    double dIn = static_cast<double>(fIn) * 2147483647.0;
    dIn = std::min(2147483647.0, std::max(-2147483648.0, dIn));
    tOut = static_cast<int32_t>(dIn);
}

template <typename T>
bool AudioProcessor::writeWaveFile(const char* szFileName, float* floatData, int32_t nNumSamples, int16_t nNumChannels, int32_t nSampleRate) {
    
    
    FILE* File = fopen(szFileName, "w+b");
    if (!File) {
        return false;
    }
    int32_t nBitsPerSample = sizeof(T) * 8;
    int nDataSize = nNumSamples * sizeof(T);

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

    // update to write
    T* pData = new T[nNumSamples];
    for (int i = 0; i < nNumSamples; ++i) {
        convFromFloat(floatData[i], pData[i]);
    }

    fwrite(pData, nDataSize, 1, File);
    delete[] pData;

    fclose(File);
    return true;
}

template bool AudioProcessor::writeWaveFile<int32_t>(const char*, float*, int32_t, int16_t, int32_t);
