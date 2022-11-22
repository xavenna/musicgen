#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <functional>
#include <SFML/Audio.hpp>
#include "music-core.h"
void printSD(ScaleDegree::ScaleDegree n) {
  std::cout << "d" << n-1;
}
void printN(Note::Note n) {
  switch(n) {
  case Note::A:
    std::cout << "A";
    break;
  case Note::AA:
    std::cout << "AA";
    break;
  case Note::B:
    std::cout << "B";
    break;
  case Note::C:
    std::cout << "C";
    break;
  case Note::CC:
    std::cout << "CC";
    break;
  case Note::D:
    std::cout << "D";
    break;
  case Note::DD:
    std::cout << "DD";
    break;
  case Note::E:
    std::cout << "E";
    break;
  case Note::F:
    std::cout << "F";
    break;
  case Note::FF:
    std::cout << "FF";
    break;
  case Note::G:
    std::cout << "G";
    break;
  case Note::GG:
    std::cout << "GG";
    break;
  case Note::Null:
    std::cout << "";
    break;
  }
}
bool Beat::operator[](int n) {
  return sounds[n];
}
Beat::Beat() : sounds(NUM_OF_INSTRUMENTS) {
}
Beat Pattern::operator[](int n) {
  return beats[n];
}
void Pattern::printContents() const {
  for(size_t i=0;i<beats.size();i++) {
    Beat x = beats.at(i);
    std::cout << "Beat " << i << ":  ";
    std::cout << "bass:" << x.sounds[0];
    std::cout << ", snare:" << x.sounds[1];
    std::cout << ", hihat:" << x.sounds[2];
    std::cout << ", crash:" << x.sounds[3];
    std::cout << ", ride:" << x.sounds[4];
    std::cout << ", open hihat:" << x.sounds[5];
    std::cout << ", melody rhythm:" << x.sounds[6];
    std::cout << ", A:" << x.sounds[7];
    std::cout << ", AA:" << x.sounds[8];
    std::cout << ", B:" << x.sounds[9];
    std::cout << ", C:" << x.sounds[10];
    std::cout << ", CC:" << x.sounds[11];
    std::cout << ", D:" << x.sounds[12];
    std::cout << ", DD:" << x.sounds[13];
    std::cout << ", E:" << x.sounds[14];
    std::cout << ", F:" << x.sounds[15];
    std::cout << ", FF:" << x.sounds[16];
    std::cout << ", G:" << x.sounds[17];
    std::cout << ", GG:" << x.sounds[18];
    //other instruments here
    std::cout << '\n';
  }
}
void Pattern::merge(const Pattern& q, const Instrument& r, const int& s) {
  size_t n = (this->beats.size()>q.beats.size()?q.beats.size():this->beats.size());
  n = (n>static_cast<size_t>(s)?s:n);
  for(size_t i=0;i<n;i++) {
    this->beats[i].sounds[r] = q.beats[i].sounds[r];
  }
}
Pattern::Pattern(int n) {
  beats.resize(n);
  length = n;
}
Pattern::Pattern() {
  length = 0;
}
Pattern Section::operator[](int n) {
  return contents[n];
}
void Section::initialize(int measuresToGenerate, int measureLength) {
  contents.resize(measuresToGenerate);
  for(auto x : contents) {
    x.beats.resize(measureLength);
  }
}
Section Rhythm::operator[](int n) {
  return contents[n];
}
void Rhythm::Play() {
  for(auto x : contents) {
    //x is a 'Section'
    //iterate through x, play each pattern
    for(auto y : x.contents) {
      //y is a pattern
      for(auto z : y.beats) {
	//z is a beat
	for(size_t i=0;i<z.sounds.size();i++) {
	  //find all sounds that play
	  bool w = z.sounds[i];
	  if(w) {
	    //i is the instrument
	    sfont.assocList[i].play();
	  }
	}
	sf::sleep(sf::seconds(60.0f/tempo));  //sleep based on tempo
      }
    }
  }
}

