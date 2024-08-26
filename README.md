# MusicXML to WAV Converter

Main function is conversion of MusicXML files to WAV audio files. The DSP side was fairly straightforward, then wanted to plug in sheet music using CV and things spiralled into electronic representation instead.

Examples can be found in output/mxml for most recent work. I've saved more than a few at this point.
A lot to do: Parser is naive and can't handle player aliases, arpeggiation, clef variations yet. On audio side, need to implement enveloping and attack timing.


# TODO:
1) Handle note transfer popping, straightforward post enveloping
7) Define facilities for bpm conversion to sampling rate, C4 to current working, 
9) Find any mxml from Bach WTC

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
2. Update the `inputMxml` variable in `main.cpp` with your filename (without extension)
3. Compile and run the program
4. The output WAV file will be generated in the `output/` directory
