#ifndef MUSIC_CORE_H
#define MUSIC_CORE_H

#define NUM_OF_INSTRUMENTS 19
//change this depending on the number of instruments
enum Instrument {BassDrum=0,SnareDrum=1,HiHatDrum=2,CrashCymbal=3,RideCymbal=4,OpenHiHat=5,MelodyRhythm=6,MelodyA,MelodyAA,MelodyB,MelodyC,MelodyCC,MelodyD,MelodyDD,MelodyE,MelodyF,MelodyFF,MelodyG,MelodyGG};
namespace Note {
  enum Note {A=0,AA=1,B=2,C=3,CC=4,D=5,DD=6,E=7,F=8,FF=9,G=10,GG=11, Null=-1};
}
namespace ScaleDegree {
  enum ScaleDegree {d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12, Null=-1};
}
void printSD(ScaleDegree::ScaleDegree);
void printN(Note::Note);
class SoundFont {
 public:
  std::vector<sf::Sound> assocList;
};
class Beat {
 public:
  //a single beat of music. Contains a list of which instruments play
  std::vector<bool> sounds;
  bool operator[](int);
  Beat();
};
class Pattern {
 public:
  //a relatively short series of beats
  size_t length;  //how long the pattern is
  std::vector<Beat> beats;
  Beat operator[](int);
  void printContents() const;
  void merge(const Pattern&, const Instrument&, const int&);
  Pattern(int);
  Pattern();
};
class Section {
 public:
  //a series of Patterns
  std::vector<Pattern> contents;
  Pattern operator[](int);
  void initialize(int, int);
};
class Rhythm {
 public:
  std::vector<Section> contents;
  Section operator[](int);
  SoundFont sfont;
  float tempo;  //measured in bpm
  void Play();
};
#endif
