#include "wavefile.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "mxml_parser.hpp"

//////////////////////////////
//////////////////////////////
//////////////////////////////

AudioProcessor::AudioProcessor() {
}

AudioProcessor::~AudioProcessor() {};

AudioProcessor::AudioProcessor(std::unordered_map<std::string, int> config) : config(config) {
}

void AudioProcessor::setConfig(std::unordered_map<std::string, int> newConfig) {
    config = newConfig;
}

void AudioProcessor::setInput(std::string sInputFilename) {
    inputFilename = genFileName(sInputFilename);
}

float AudioProcessor::getFreq(int octave, int note) const {
    // TODO - memoize? may be makework
    return (float)(440 * pow(2.0, ((double)((octave - 4) * 12 + note)) / 12.0));
}

std::string AudioProcessor::genFileName(std::string& fn) const {
    auto now = std::chrono::system_clock::now();
    long now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "output/" << fn << "_" << now_c << ".wav";
    return ss.str();
}

int AudioProcessor::maxMeasure(std::vector<Part>& mxml) const {
    // TODO - assume last measure is latest
    int maxM = 0;
    for (Part& x : mxml) {
        for (Measure& y : x.measures) {
            maxM = std::max(maxM, y.measurePos);
        }
    }
    return maxM;
}

std::unordered_map<int, int> AudioProcessor::getWeights(std::vector<Part>& mxml, std::unordered_map<std::string, int>& config) const {
    // calculate relevant weightings of notes (more means cut amplitude to avoid clipping)
    std::unordered_map<int, int> partWeight{{0, 0}};
    int currMeasurePos;
    int baseNoteLength;
    int chordEnd, chordCard;
    for (Part& currPart : mxml) {
        for (Measure& currMeasure : currPart.measures) {
            currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];                                     // mxml is 1-indexed
            baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;  // if 4, on
            for (Chord& currChord : currMeasure.chords) {
                chordEnd = currMeasurePos + (currChord.duration * baseNoteLength);
                chordCard = currChord.octNotes.size();  // number notes in chord

                // TODO - Necessary with current sequential part parsing approach, change to skips on rfx
                for (int i = currMeasurePos; i < chordEnd; i++) {
                    if (partWeight.find(i) == partWeight.end()) {
                        partWeight[i] = 0;
                    }
                    partWeight[i] += chordCard;
                }
                currMeasurePos = chordEnd;
            }
        }
    }
    return partWeight;
}

void AudioProcessor::genWaveform(std::vector<Part>& mxml) {
    // additional params
    int measureCount = maxMeasure(mxml);
    std::unordered_map<int, int> partWeight = getWeights(mxml, config);

    int nDataL = measureCount * config["nSampleRate"] * config["nNumChannels"];
    this->waveform.resize(nDataL, 0.0f);
    config["nNumSamples"] = nDataL;

    // add audio
    int currMeasurePos, baseNoteLength;
    int chordStart, chordEnd, chordLen;
    int currNote = 0;

    //TODO - bush league enveloping
    int stepIn, stepOut;
    

    for (Part& currPart : mxml) {
        for (Measure& currMeasure : currPart.measures) {
            currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];                                     // mxml is 1-indexed
            baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;  // if 4, on

            for (Chord& currChord : currMeasure.chords) {
                chordLen = currChord.duration * baseNoteLength;
                chordStart = currMeasurePos;
                chordEnd = currMeasurePos + chordLen; 

                //TODO - bush league enveloping
                stepIn = chordLen/400;
                stepOut = chordLen/20;
                

                std::vector<float> chordFreq{};
                for (std::pair<int, std::string>& currNote : currChord.octNotes) {
                    //TODO - rfx keyMap to within parser. Should have done this earlier
                    std::pair<int, int> temp {currNote.first, keyMap[currNote.second]};
                    if (freqCache.find(temp) == freqCache.end()){
                        freqCache[temp] = getFreq(currNote.first, keyMap[currNote.second]);
                    }
                    chordFreq.push_back(freqCache[temp]);
                }
                for (int i = chordStart; i <= chordEnd; ++i) {
                    float t = static_cast<float>(i) / config["nSampleRate"];
                    float norm = 1.0f / partWeight[i];
                    float adj = 1.0f;

                    //TODO - bushleague enveloping
                    if (i < chordStart + stepIn) {
                        adj = 1.0f / stepIn * (i - chordStart);
                    } else if (chordEnd - stepOut < i) {
                        adj = 1.0f / stepOut * (chordEnd-i);
                    } else {
                        adj = 1.0f;
                    }

                    for (float& currNote : chordFreq) {
                        this->waveform[i] += adj * norm * std::sin(2 * M_PI * currNote * t);
                    }
                }
                currMeasurePos = chordEnd;
            }
        }
    }
}

bool AudioProcessor::writeWaveFile() {
    // TODO - Don't look too hard
    return AudioProcessor::writeWaveFile<int32_t>(
        config["nNumSamples"],  // note nNumSamples is added  in mxmlFac
        config["nNumChannels"],
        config["nSampleRate"]);
}

template <typename T>
bool AudioProcessor::writeWaveFile(int nNumSamples, int nNumChannels, int nSampleRate) {
    FILE* File = fopen(this->inputFilename.c_str(), "w+b");
    if (!File) {
        std::cout << "not file: " << inputFilename.c_str() << std::endl;
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
        convFromFloat(this->waveform[i], pData[i]);
    }

    fwrite(pData, nDataSize, 1, File);
    delete[] pData;

    fclose(File);
    return true;
}

void AudioProcessor::convFromFloat(float fIn, int32_t& tOut) {
    double dIn = static_cast<double>(fIn) * 2147483647.0;
    dIn = std::min(2147483647.0, std::max(-2147483648.0, dIn));
    tOut = static_cast<int32_t>(dIn);
}


// namespace std {
//     template <>
//     struct hash<std::pair<int, int>> {
//         size_t operator()(const std::pair<int, int>& p) const {
//             return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
//         }
//     };
// }

// void AudioProcessor::genWaveform(std::vector<Part>& mxml) {
//     int measureCount = maxMeasure(mxml);
//     std::unordered_map<int, int> partWeight = getWeights(mxml, config);

//     int nDataL = measureCount * config["nSampleRate"] * config["nNumChannels"];
//     this->waveform.resize(nDataL, 0.0f);
//     config["nNumSamples"] = nDataL;

//     int currMeasurePos, baseNoteLength;
//     int chordStart, chordEnd, chordLen;

//     // Pre-calculate frequencies
//     std::unordered_map<std::pair<int, int>, double> freqCache;

//     for (Part& currPart : mxml) {
//         for (Measure& currMeasure : currPart.measures) {
//             currMeasurePos = (currMeasure.measurePos - 1) * config["nSampleRate"];
//             baseNoteLength = config["nSampleRate"] * currMeasure.attributes.divisions / currMeasure.attributes.beats;

//             for (Chord& currChord : currMeasure.chords) {
//                 chordLen = currChord.duration * baseNoteLength;
//                 chordStart = currMeasurePos;
//                 chordEnd = currMeasurePos + chordLen;
//                 float attack = chordLen * 0.01f;
//                 float release = chordLen * 0.05f;

//                 std::vector<std::pair<double, double>> chordFreqs; // freq, phase
//                 for (std::pair<int, std::string>& currNote : currChord.octNotes) {
//                     int octave = currNote.first;
//                     int note = keyMap[currNote.second];
//                     auto key = std::make_pair(octave, note);
//                     if (freqCache.find(key) == freqCache.end()) {
//                         freqCache[key] = 440.0 * std::pow(2.0, (double)((octave - 4) * 12 + note) / 12.0);
//                     }
//                     chordFreqs.emplace_back(freqCache[key], (double)rand() / RAND_MAX * 2 * M_PI);
//                 }

//                 for (int i = chordStart; i < chordEnd; ++i) {
//                     double t = static_cast<double>(i - chordStart) / config["nSampleRate"];
//                     double envelope = 1.0;
//                     if (i - chordStart < attack) {
//                         envelope = (i - chordStart) / attack;
//                     } else if (chordEnd - i < release) {
//                         envelope = (chordEnd - i) / release;
//                     }

//                     double sample = 0.0;
//                     for (const auto& [freq, phase] : chordFreqs) {
//                         sample += std::sin(2 * M_PI * freq * t + phase);
//                     }
//                     sample *= envelope / chordFreqs.size(); 

//                     this->waveform[i] += static_cast<float>(sample);
//                 }
//                 currMeasurePos = chordEnd;
//             }
//         }
//     }

//     // Apply a simple limiter to prevent clipping
//     float maxAmp = *std::max_element(this->waveform.begin(), this->waveform.end(),
//                                      [](float a, float b) { return std::abs(a) < std::abs(b); });
//     if (maxAmp > 1.0f) {
//         for (float& sample : this->waveform) {
//             sample /= maxAmp;
//         }
//     }
// }