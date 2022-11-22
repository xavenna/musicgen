#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <fstream>
#include <algorithm>
#include <functional>
#include <SFML/Audio.hpp>

#include "music-core.h"
#include "markov.h"
#include "musicgen.h"
#include "miscutil.h"
#include "musicgen-util.h"
/* This file contains the driver code for the generation.
 * Version 1.0.0
 * major.minor.patch
 */
/* TODO:
 * $$ encapsulate generation inside something
 * simplify rhythms to be playable on drumset
 * * in a second pass, remove some simultaneous notes
 * $$ make hihat and crash cymbal sounds better
 * improve markov chains
 * add the rest of drumset instruments: [toms, other cymbals]
 * add pitched instrument(s)
 *** Make algorithm for creating not bad sounding music
 * $$song name generator
 * make it so measure length is variable 
 * rewrite argument parsing to use fewer try catch blocks
 *
 * EVENTUALLY:
 * randomly generate markov chains
 * (maybe use machine learning to weigh connections?)
 */
int main(int argc, char** argv) {
  ////////////////////
  // parse arguments
  ////////////////////
  //variables needed for arguments
  int measuresToGenerate = 5;
  int preMeasures = 2;
  int beatsPerMeasure = 8;
  bool debugMode = false;
  bool putSong = false;
  bool loadSong = false;
  std::string loadFileName;
  std::string expFileName;
  //parsing
  for(int i=1;i<argc;i++) {
    if(argc==1)
      break;
    std::string arg{argv[i]};
    if(arg.substr(0,2) == "-n") {
      if(arg.size() > 2) {
	try {
	  measuresToGenerate = std::stoi(arg.substr(2));
	}
	catch (...) {
	  std::cout << "Error parsing argument '" << arg << "'.\n";
	}
      }
    }
    else if(arg.substr(0,2) == "-l") {  //num measures pre-generated and thrown away
      if(arg.size() > 2) {
	try {
	  preMeasures = std::stoi(arg.substr(2));
	}
	catch (...) {
	  std::cout << "Error parsing argument '" << arg << "'.\n";
	}
      }
    }
    else if(arg.substr(0,2) == "-b") {  //beats per measure
      if(arg.size() > 2) {
	try {
	  beatsPerMeasure = std::stoi(arg.substr(2));
	}
	catch (...) {
	  std::cout << "Error parsing argument '" << arg << "'.\n";
	}
      }
    }
    else if(arg.substr(0,2) == "-e") {
      if(arg.size() > 2) {
	expFileName = arg.substr(2);
	putSong = true;
      }
    }
    else if(arg.substr(0,2) == "-p") {
      if(arg.size() > 2) {
	loadFileName = arg.substr(2);
	loadSong = true;
      }
    }
    else if(arg == "-d") {
      debugMode = true;
    }
    else if(arg == "-h") {
      //help
      std::cout << "Musicgen v1.0.0.\tArguments:\n";
      std::cout << "-b:\tSets beats-per-measure.\t\t\tUsage: -b<beats per measure>\n";
      std::cout << "-d:\tEnables debug mode.\t\t\tUsage: -d\n";
      std::cout << "-e:\tExport song to file.\t\t\tUsage: -e<filename>\n";
      std::cout << "-h:\tDisplay this help menu.\t\t\tUsage: -h\n";
      std::cout << "-l:\tSet number of pre-generated measures.\tUsage: -l<number>\n";
      std::cout << "-n:\tSet number of measures to generate.\tUsage: -n<measures>\n";
      std::cout << "-p:\tLoad song from file.\t\t\tUsage: -p<filename>\n";
      return 0;
    }
    else {
      std::cout << "Error: Unrecognised argument `" << arg << "`. Use `musicgen -h' to see a list of arguments.\n";
    }
  }
  /////////////////////////
  // initialize variables
  /////////////////////////
  //initialize sound font
  SoundFont sf1;
  sf::SoundBuffer buf_bassdrum;
  sf::SoundBuffer buf_snaredrum;
  sf::SoundBuffer buf_hihat;
  sf::SoundBuffer buf_crashcymbal;
  sf::SoundBuffer buf_ridecymbal;
  sf::SoundBuffer buf_openhihat;
  sf::SoundBuffer buf_melody_tone;
  sf::SoundBuffer buf_melody_a;
  sf::SoundBuffer buf_melody_aa;
  sf::SoundBuffer buf_melody_b;
  sf::SoundBuffer buf_melody_c;
  sf::SoundBuffer buf_melody_cc;
  sf::SoundBuffer buf_melody_d;
  sf::SoundBuffer buf_melody_dd;
  sf::SoundBuffer buf_melody_e;
  sf::SoundBuffer buf_melody_f;
  sf::SoundBuffer buf_melody_ff;
  sf::SoundBuffer buf_melody_g;
  sf::SoundBuffer buf_melody_gg;
  buf_bassdrum.loadFromFile("assets/bassdrum.wav");
  buf_snaredrum.loadFromFile("assets/snaredrum.wav");
  buf_hihat.loadFromFile("assets/hihat.wav");
  buf_crashcymbal.loadFromFile("assets/crashcymbal.wav");
  buf_ridecymbal.loadFromFile("assets/ridecymbal.wav");
  buf_openhihat.loadFromFile("assets/ridecymbal.wav");
  buf_melody_tone.loadFromFile("assets/dong01.wav");
  buf_melody_a.loadFromFile("assets/melodya.wav");
  buf_melody_aa.loadFromFile("assets/melodyaa.wav");
  buf_melody_b.loadFromFile("assets/melodyb.wav");
  buf_melody_c.loadFromFile("assets/melodyc.wav");
  buf_melody_cc.loadFromFile("assets/melodycc.wav");
  buf_melody_d.loadFromFile("assets/melodyd.wav");
  buf_melody_dd.loadFromFile("assets/melodydd.wav");
  buf_melody_e.loadFromFile("assets/melodye.wav");
  buf_melody_f.loadFromFile("assets/melodyf.wav");
  buf_melody_ff.loadFromFile("assets/melodyff.wav");
  buf_melody_g.loadFromFile("assets/melodyg.wav");
  buf_melody_gg.loadFromFile("assets/melodygg.wav");

  sf1.assocList.push_back(sf::Sound(buf_bassdrum));
  sf1.assocList.push_back(sf::Sound(buf_snaredrum));
  sf1.assocList.push_back(sf::Sound(buf_hihat));
  sf1.assocList.push_back(sf::Sound(buf_crashcymbal));
  sf1.assocList.push_back(sf::Sound(buf_ridecymbal));
  sf1.assocList.push_back(sf::Sound(buf_openhihat));
  sf1.assocList.push_back(sf::Sound(buf_melody_tone));
  sf1.assocList.push_back(sf::Sound(buf_melody_a));
  sf1.assocList.push_back(sf::Sound(buf_melody_aa));
  sf1.assocList.push_back(sf::Sound(buf_melody_b));
  sf1.assocList.push_back(sf::Sound(buf_melody_c));
  sf1.assocList.push_back(sf::Sound(buf_melody_cc));
  sf1.assocList.push_back(sf::Sound(buf_melody_d));
  sf1.assocList.push_back(sf::Sound(buf_melody_dd));
  sf1.assocList.push_back(sf::Sound(buf_melody_e));
  sf1.assocList.push_back(sf::Sound(buf_melody_f));
  sf1.assocList.push_back(sf::Sound(buf_melody_ff));
  sf1.assocList.push_back(sf::Sound(buf_melody_g));
  sf1.assocList.push_back(sf::Sound(buf_melody_gg));
  sf1.assocList[3].setVolume(60.0f);
  if(debugMode)
    std::cout << "Sound initialization complete\n";
  //initialize markov chains
  MarkovChain bassDrum;
  //bassDrumInitialize(bassDrum);
  bassDrum.initialize("assets/bass.mkc", BassDrum);
  MarkovChain snareDrum;
  //snareDrumInitialize(snareDrum);
  snareDrum.initialize("assets/snare.mkc", SnareDrum);
  MarkovChain hiHat;
  //hiHatInitialize(hiHat);
  hiHat.initialize("assets/hihat.mkc", HiHatDrum);
  MarkovChain crashCymbal;
  //crashCymbalInitialize(crashCymbal);
  crashCymbal.initialize("assets/crash.mkc", CrashCymbal);
  MarkovChain rideCymbal;
  //rideCymbalInitialize(rideCymbal);
  rideCymbal.initialize("assets/ride.mkc", RideCymbal);
  MarkovChain openHiHat;
  //openHiHatInitialize(openHiHat);
  openHiHat.initialize("assets/ohihat.mkc", OpenHiHat);
  MarkovChain melodyRhythm;
  //melodyRhythmInitialize(melodyRhythm);
  melodyRhythm.initialize("assets/mrhythm.mkc", MelodyRhythm);
  ChainList listOfInstruments;
  listOfInstruments.append(&bassDrum).append(&snareDrum).append(&hiHat).append(&crashCymbal).append(&rideCymbal).append(&openHiHat).append(&melodyRhythm);
  MelodicMarkovChain melodyChain;
  //melodyInitialize(melodyChain);
  melodyChain.initialize("assets/melody.mkc");
  if(debugMode)
    std::cout << "Markov chain initialization complete\n";
  /*
  for(size_t i=0;i<snareDrum.map.size();i++) {
    auto x = snareDrum.map[i];
    for(size_t j=0;j<x.size();j++) {
      std::cout << i << ',' << j << ": ";
      auto y = x[j];
      for(auto z : y) {
	std::cout << z.first << ':' << z.second << ' ';
      }
      std::cout << '\n';
    }
  }
  */
  //other initialization
  Rhythm r1;
  r1.sfont = sf1;
  r1.tempo = 240.0f;
  Section sect1;  //the section used for this test
  std::ofstream put;
  std::ifstream load;
  std::string songTitle;
  if(putSong)
    put.open(expFileName);
  if(loadSong)
    load.open(loadFileName);
  //only do if not loading song
  if(!loadSong) {
    //////////////////////
    // rhythm generation
    //////////////////////
    
    ///generate sections and stitch them together
    
    //use markov chaining to link together patterns
    //set initial nodes
    bassDrum.setCurrentNode(1);
    snareDrum.setCurrentNode(0);
    hiHat.setCurrentNode(0);
    crashCymbal.setCurrentNode(4);
    rideCymbal.setCurrentNode(1);
    openHiHat.setCurrentNode(2);
    melodyRhythm.setCurrentNode(0);
    melodyChain.setCurrentNode(0);

    generateSection(listOfInstruments, sect1, measuresToGenerate, preMeasures, beatsPerMeasure);

    songTitle = getSongName();
    std::cout << "Song title: " << songTitle << ".\n";
    /////////////////////////////////
    // combine sections into rhythm
    /////////////////////////////////
    r1.contents.push_back(sect1);
  }
  else {  //if loading song
    std::string temp;
    int lines = 0;
    std::getline(load, temp);
    std::string name = temp;
    std::getline(load, temp);
    int measureBeats = std::stoi(temp);
    int pattC = 0;
    sect1.contents.push_back(Pattern(measureBeats));
    while(load.peek() != EOF) {
      std::vector<std::string> bools;
      std::getline(load,temp);
      sSplit(temp, bools, ",");
      for(size_t i=0;i<bools.size();i++) {
	sect1.contents.back().beats[lines%measureBeats].sounds[i] = (std::stoi(bools[i]));
      }
      lines++;
      if(lines%measureBeats == 0) {
	pattC++;
	if(load.peek() != EOF)
	  sect1.contents.push_back(Pattern(measureBeats));
      }
    }
    r1.contents.push_back(sect1);
    std::cout << "Song name: " << name << "\n";
  }
  ///////////////////
  // process rhythm
  ///////////////////
  //put this in a separate file or something
  if(!loadSong) {
    auto beatFix = [](Beat& b){
      //find mutually exclusive beats and fix them
      //conflicts: open/close hihat
      //ride/crash/snare (only 2 of 3)
      if(b[2] && b[5]) { //open and close hihat
	b.sounds[2] = getRandInt(0,2);
	b.sounds[5] = !b[2];
      }
      if(b[1] && b[3] && b[4] && (b[2] || b[5])) { //sn cr ri
	int ti = getRandInt(0,4);
	b.sounds[1] = (ti != 3);
	b.sounds[3] = (ti != 2);
	b.sounds[4] = (ti != 1);
	if(b.sounds[2] == 1)
	  b.sounds[2] = (ti != 0);
	else
	  b.sounds[5] = (ti != 0);
      }
    };
    auto paIt = [beatFix](Pattern& p){std::for_each(p.beats.begin(),p.beats.end(),beatFix);};
    auto fIt = [paIt,beatFix](Section& s){std::for_each(s.contents.begin(),s.contents.end(),paIt);};
    auto rIt = [fIt,paIt,beatFix](Rhythm& r){std::for_each(r.contents.begin(),r.contents.end(),fIt);};
    rIt(r1);
    //next, process melody rhythm.
    ///not sure how this will  work, yet
    int numBeats = r1.contents.size() * measuresToGenerate * beatsPerMeasure;
    std::vector<ScaleDegree::ScaleDegree> melody;
    melody.resize(numBeats);
    std::vector<bool> rhythm(numBeats);
    std::vector<Note::Note> tmelody(numBeats);
    tmelody.resize(numBeats);
    int counter = 0;
    for(auto x : r1.contents) {
      for(auto y : x.contents) {
	for(auto z : y.beats) {
	  rhythm[counter] = z[MelodyRhythm];
	  counter++;
	}
      }
    }
    if(debugMode) {
      std::cout << "Melodic pattern:\n";
      std::for_each(rhythm.begin(),rhythm.end(),[](bool e){std::cout << e << ",";});
      std::cout << "\n";
    }
    //transform it into a melodic line
    
    //steps: choose key
    ///for now, just use a minor

    //maybe use more markov chains for generating melody?!!
    generateMelody(melodyChain, rhythm, melody);
    if(debugMode) {
      std::cout << "Melodic contour:\n";
      std::for_each(melody.begin(),melody.end(),[](ScaleDegree::ScaleDegree n){printSD(n);std::cout << ",";});
      std::cout << "\n";
    }
    resolveKey(melody, tmelody, Note::A);
    if(debugMode) {
      std::cout << "Melody:\n";
      std::for_each(tmelody.begin(),tmelody.end(),[](Note::Note n){printN(n);std::cout << ",";});
      std::cout << "\n";
    }
    counter = 0;
    for(auto& x : r1.contents) {
      for(auto& y : x.contents) {
	for(auto& z : y.beats) {
	  if(tmelody[counter] != Note::Null) {
	    z.sounds[7+tmelody[counter]] = true;
	  }
	  z.sounds[6] = false;
	  counter++;
	}
      }
    }
    //clear melody rhythm
  }
  //////////////////////////
  // play generated rhythm
  //////////////////////////
  if(debugMode)  //reads out contents of played measures, for debugging purposes
    for(auto x : sect1.contents) {x.printContents();}
  r1.Play();
  ///////////////////////
  // put song to file
  ///////////////////////
  //each line is a comma delimited list of instruments
  if(putSong) {
    put << songTitle << '\n';
    put << r1.contents.back().contents.back().beats.size() << '\n';
    for(Section x : r1.contents) {
      for(Pattern y : x.contents) {
	for(Beat z : y.beats) {
	  for(bool w : z.sounds) {
	    put << w << ',';
	  }
	  put << "\n";
	}
      }
    }
  }
}

void markovGenerateInstrument(Section& sect, MarkovChain& mc, int measuresToGenerate, Instrument inst) {
  for(int i=0;i<measuresToGenerate;i++) {
    mc.selectNextNode();
    sect.contents[i].merge(mc.contents[mc.current].pattern, inst, 8);
  }
}
