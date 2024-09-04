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
#include <sstream>

#include "mxml_parser.hpp"

class AudioProcessor {
   private:
        std::unordered_map<std::string, int> keyMap;
        int sampleRate;
        int numChannels;
        float volume;
        std::unordered_map<std::string, int> config;

        float getFreq(int octave, int note);
        std::string genFileName(std::string& fn);
        int maxMeasure(std::vector<Part>& mxml);
        std::unordered_map<int,int> getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config);
        std::pair<std::string, std::string> inputOutput;

   public:
        AudioProcessor();
        AudioProcessor(std::unordered_map<std::string, int> config);
        ~AudioProcessor();
        void setSampleRate(int newSampleRate);
        void setNumChannels(int newNumChannels);
        void setVolume(int newVolume);
        void setConfig(std::unordered_map<std::string, int> config);
        void setInputOutput(std::string baseName);
};

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


void AudioProcessor::setSampleRate(int newSampleRate) {
    sampleRate = newSampleRate;
}

void AudioProcessor::setNumChannels(int newNumChannels) {
    numChannels = newNumChannels;
}

void AudioProcessor::setVolume(int newVolume) {
    volume = newVolume;
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

