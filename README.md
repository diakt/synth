# MusicXML to WAV Converter (currently rfx)

Conversion of MusicXML files (sheet music) to WAV audio files. The DSP audio generation side was fairly straightforward, but then wanted to plug in sheet music using CV (essentially OMR), and things spiralled rapidly.

Examples can be found in output/mxml for most recent work. I've saved more than a few at this point.
A lot to do: Parser is naive and can't handle player aliases, arpeggiation, clef variations yet. On audio side, need to implement enveloping and attack timing. Trying to avoid prebuilt synths.

In the longer term, I suspect I'm going to end up with functionality similar to [this paper](https://firebasestorage.googleapis.com/v0/b/afika-nyati-website.appspot.com/o/design%2Fcadencv%2Fcadencv_afika_nyati.pdf?alt=media&token=a5aa2413-32c0-4bc7-8222-06342b822096), but that was a post-hoc find and I'm moving pretty quickly.


# TODO:
0) Rfx almost everything
1) Handle note transfer popping, straightforward post enveloping
3) Extend factory support to non-sine
7) Define facilities for bpm conversion to sampling rate.
8) Add writes for non-wav (mp4, OPUS)
9) Find any mxml from Bach WTC
10) OpenCV linking

## Features

- Parses MusicXML files using libxml2
- Synthesizes audio waveforms based on musical notation
- Supports various waveform types (sine, square, saw, triangle, noise)
- Generates WAV files with configurable sample rate and channel count
- Handles multiple parts and complex chord structures

## Components

1. `mxml_parser.hpp` / `mxml_parser.cpp`: Parses MusicXML files into a structured representation
2. `wavefile.hpp` / `wavefile.cpp`: Handles WAV file generation
3. `main.cpp`: Orchestrates the conversion process

## Dependencies

- libxml2: For parsing XML files
- C++11 
- Musicxml: Recommend reading through [the documentation](https://www.musicxml.com/for-developers/) to get up to speed.

## Usage

1. Place your MusicXML file in the `./res/mxml/tests/` directory (or don't, I would recommend anything already there).
2. Update the `inputMxml` variable in `main.cpp` with your filename (sans extension)
3. Compile and run the program
4. The output WAV file will be generated in the `output/` directory
