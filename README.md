# synth workup

Based heavily off of demofox's work at https://blog.demofox.org/diy-synthesizer/

@ Popping aka Discont @ https://blog.demofox.org/2012/05/19/diy-synthesizer-chapter-2-common-wave-forms/


TODO (Need demos):
0) Fix timing of notes (e.g. fill full section)
1) Add chord parsing.
1) Add part and measure parsing.
2) Add oscillator to sin wave mxml interpolation
3) Add alter parsing
4) Define facilities for bpm conversion to sampling rate, C4 to current working, 
5) Make well tempered clavier song
6) Make keyboard
7) Make blue pink brown noise




# Schemas

part -> measures -> arr[chords]
measures = unordered_map<int, arr[chords]>
arr[chords] = [chord, chord, chord]
chord = std::vector<freq>
compress note note note to chord "abc" string in vec