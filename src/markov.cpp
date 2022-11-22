#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <random>
#include <fstream>

#include "music-core.h"
#include "markov.h"
#include "miscutil.h"

// This should be rewritten to be dually-predictive

//how should this work?

void MarkovNode::setContents(std::vector<bool> n, Instrument inst) {
  pattern.beats.resize(n.size());
  for(size_t i=0;i<n.size();i++) {
    pattern.beats[i].sounds.resize(NUM_OF_INSTRUMENTS);
    pattern.beats[i].sounds[inst] = n[i];
  }
}
MarkovNode::MarkovNode(int n) {
  pattern.length = n;
}
void MelodicMarkovNode::setContents(ScaleDegree::ScaleDegree n) {
  note = n;
}
MelodicMarkovNode::MelodicMarkovNode(ScaleDegree::ScaleDegree n) {
  note = n;
}
MelodicMarkovNode::MelodicMarkovNode(int n) {
  if(n > 12 || n < 0) {
    throw std::runtime_error("MelodicMarkovNode() : invalid specified degree");
  }
  note = static_cast<ScaleDegree::ScaleDegree>(n);
}
void MarkovChain::selectNextNode() { 
  //find correct connection matrix
  if(map.find(hist.x) == map.end() || map.find(hist.x)->second.find(hist.y) == map.find(hist.x)->second.end()) {
    throw std::runtime_error("MarkovChain::SelectNextNode() : ERROR: unspecified future for combination\n");
  }
  std::map<int, float> connections = map.find(hist.x)->second.find(hist.y)->second;
  hist.x = hist.y;
  hist.y = current;
  float accum = 0;
  float value = getRandFloat(0.f,1.f);
  for(auto x : connections) {
    accum += x.second;
    if(accum >= value) {
      current = x.first;
      break;
    }
  }
}
void MarkovChain::setCurrentNode(size_t n) {
  current = n;
}
MarkovNode* MarkovChain::getCurrentNode() {
  return &contents[current];
}
void MelodicMarkovChain::selectNextNode() {
  if(map.find(hist.x) == map.end() || map.find(hist.x)->second.find(hist.y) == map.find(hist.x)->second.end()) {
    throw "ERROR: unspecified future for combination\n";
  }
  std::map<int, float> connections = map.find(hist.x)->second.find(hist.y)->second;
  hist.x = hist.y;
  hist.y = current;
  float accum = 0;
  float value = getRandFloat(0.f,1.f);
  for(auto x : connections) {
    accum += x.second;
    if(accum >= value) {
      current = x.first;
      break;
    }
  } /*
  float accum = 0;
  float value = getRandFloat(0.f,1.f);
  for(auto x : currentNode->connections) {
    accum += x.second;
    if(accum >= value) {
      currentNode = x.first;
      break;
    }
    } */
}
void MelodicMarkovChain::setCurrentNode(size_t n) {
  current = n;
}
MelodicMarkovNode* MelodicMarkovChain::getCurrentNode() {
  return &contents.at(current);
}
MarkovChain& ChainList::getChain(int n) {
  return *contents.at(n);
  //done to prevent crashes
}
ChainList& ChainList::append(MarkovChain* n) {
  contents.push_back(n);
  return *this;
}

void MarkovChain::initialize(std::string fn, Instrument in) {
  instr = in;
  std::ifstream read(fn);
  std::string line;
  std::size_t counter1{0};
  std::size_t counter2{0};
  if(!read.is_open()) {
    std::cout << "Error: '" << fn << "' could not be opened\n";
    return;
  }
  while(read.peek() != EOF) {
    std::getline(read, line);
    if(line.size() == 0 || line[0] == '#')
      continue; //skip comments and empty lines
    if(line[0] == '*') {
      //first section complete
      break;
    }
    //parse line: 
    MarkovNode n(line.size());
    std::vector<bool> contents;
    for(auto x : line) {
      contents.push_back(x == '1');
    }
    n.setContents(contents, in);
    this->contents.push_back(n);
    counter1++;
  }
  while(read.peek() != EOF) {
    std::getline(read, line);
    if(!line.empty() && line[0] == '#')
      continue; //filter out comments and empty lines
    std::vector<std::string> nums;
    //this time, connections are gathered
    //split line into numbers
    sSplit(line, nums, " ");
    for(size_t i=0;i<nums.size();i++) {
      auto x = nums[i];
      if(!isNum(x)) {
	std::cout << "Error: invalid character in instrument configuration\n";
	return;
      }
      float p = std::stof(x);
      //add nums to the probability vector
      map[static_cast<int>(counter2/counter1)][counter2%counter1].emplace(i, p);
    }
    counter2++;
  }
}
void MelodicMarkovChain::initialize(const std::string fn) {
  std::ifstream read(fn);
  std::string line;
  std::size_t counter1{0};
  std::size_t counter2{0};
  if(!read.is_open()) {
    std::cout << "Error: '" << fn << "' could not be opened\n";
    return;
  }
  while(read.peek() != EOF) {
    std::getline(read, line);
    if(line.size() == 0 || line[0] == '#')
      continue; //skip comments and empty lines
    if(line[0] == '*') {
      //first section complete
      break;
    }
    MelodicMarkovNode n(std::stoi(line,0, 16));
    contents.push_back(n);
    counter1++;
  }
  while(read.peek() != EOF) {
    std::getline(read, line);
    if(!line.empty() && line[0] == '#')
      continue; //filter out comments and empty lines
    std::vector<std::string> nums;
    //this time, connections are gathered
    //split line into numbers
    sSplit(line, nums, " ");
    for(size_t i=0;i<nums.size();i++) {
      auto x = nums[i];
      if(!isNum(x)) {
	std::cout << "Error: invalid character in instrument configuration\n";
	return;
      }
      float p = std::stof(x);
      //add nums to the probability vector
      map[static_cast<int>(counter2/counter1)][counter2%counter1].emplace(i, p);
    }
    counter2++;
  }
}



/*
void hiHatInitialize(MarkovChain& mc) {
  mc.instr = HiHatDrum;
  MarkovNode h1(8);
  MarkovNode h2(8);
  MarkovNode h3(8);
  h1.setContents(std::vector<bool>{true,false,true,false,true,false,true,false},HiHatDrum);
  h2.setContents(std::vector<bool>{true,true,true,true,true,true,true,true},HiHatDrum);
  h3.setContents(std::vector<bool>{false,false,false,true,false,false,false,true},HiHatDrum);
  mc.contents.push_back(h1);
  mc.contents.push_back(h2);
  mc.contents.push_back(h3);
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.5f));

  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.5f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.5f));

  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.5f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  
}
void snareDrumInitialize(MarkovChain& mc) {
  mc.instr = SnareDrum;
  MarkovNode s1(8);
  MarkovNode s2(8);
  MarkovNode s3(8);
  MarkovNode s4(8);
  MarkovNode s5(8);
  s1.setContents(std::vector<bool>{false,false,true,false,true,false,false,true},SnareDrum);
  s2.setContents(std::vector<bool>{false,false,true,false,false,false,true,false},SnareDrum);
  s3.setContents(std::vector<bool>{false,true,false,true,false,true,false,true},SnareDrum);
  s4.setContents(std::vector<bool>{false,true,true,false,false,true,true,false},SnareDrum);
  s5.setContents(std::vector<bool>{true,true,false,false,false,true,false,true},SnareDrum);
  mc.contents.push_back(s1);
  mc.contents.push_back(s2);
  mc.contents.push_back(s3);
  mc.contents.push_back(s4);
  mc.contents.push_back(s5);

  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.25f));

  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.25f));

  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.25f));

  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.25f));

  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.25f));
}
void bassDrumInitialize(MarkovChain& mc) {
  mc.instr = BassDrum;
  MarkovNode b1(8);
  MarkovNode b2(8);
  MarkovNode b3(8);
  MarkovNode b4(8);
  MarkovNode b5(8);
  MarkovNode b6(8);
  MarkovNode b7(8);
  MarkovNode b8(8);
  b1.setContents(std::vector<bool>{true,true,true,true,false,false,false,false},BassDrum);
  b2.setContents(std::vector<bool>{true,true,true,true,true,true,true,true},BassDrum);
  b3.setContents(std::vector<bool>{true,true,true,false,true,true,true,false},BassDrum);
  b4.setContents(std::vector<bool>{true,true,false,true,false,true,true,false},BassDrum);
  b5.setContents(std::vector<bool>{true,true,false,true,true,true,false,true},BassDrum);
  b6.setContents(std::vector<bool>{true,false,true,false,true,false,true,false},BassDrum);
  b7.setContents(std::vector<bool>{true,true,true,true,false,true,true,true},BassDrum);
  b8.setContents(std::vector<bool>{true,true,true,true,false,false,false,true},BassDrum);
  mc.contents.push_back(b1);
  mc.contents.push_back(b2);
  mc.contents.push_back(b3);
  mc.contents.push_back(b4);
  mc.contents.push_back(b5);
  mc.contents.push_back(b6);
  mc.contents.push_back(b7);
  mc.contents.push_back(b8);

  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.5f));

  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.2f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.2f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.2f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.2f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.2f));

  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.33f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.34f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.33f));

  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.2f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.2f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.2f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.2f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.2f));

  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.5f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.5f));

  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.33f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.34f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.33f));

  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.25f));

  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.5f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  
}
void rideCymbalInitialize(MarkovChain& mc) {
  mc.instr = RideCymbal;
  MarkovNode c1(8);
  MarkovNode c2(8);
  MarkovNode c3(8);
  c1.setContents(std::vector<bool>{true,false,true,false,true,false,true,false},RideCymbal);
  c2.setContents(std::vector<bool>{false,false,false,false,false,false,false,false},RideCymbal);
  c3.setContents(std::vector<bool>{true,true,true,true,true,true,true,true},RideCymbal);
  mc.contents.push_back(c1);
  mc.contents.push_back(c2);
  mc.contents.push_back(c3);
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.35f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.6f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.65f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.3f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.15f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.4f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.45f));
}
void openHiHatInitialize(MarkovChain& mc) {
  mc.instr = OpenHiHat;
  MarkovNode h1(8);
  MarkovNode h2(8);
  MarkovNode h3(8);
  h1.setContents(std::vector<bool>{false,true,false,false,false,true,false,false},OpenHiHat);
  h2.setContents(std::vector<bool>{false,false,false,false,false,false,false,false},OpenHiHat);
  h3.setContents(std::vector<bool>{false,false,false,false,false,false,false,true},OpenHiHat);
  mc.contents.push_back(h1);
  mc.contents.push_back(h2);
  mc.contents.push_back(h3);
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.5f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.35f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.4f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.25f));
}
void crashCymbalInitialize(MarkovChain& mc) {
  mc.instr = CrashCymbal;
  MarkovNode c1(8);
  MarkovNode c2(8);
  MarkovNode c3(8);
  MarkovNode c4(8);
  MarkovNode c5(8);
  c1.setContents(std::vector<bool>{true,false,false,false,true,false,false,false},CrashCymbal);
  c2.setContents(std::vector<bool>{false,false,false,false,false,false,false,false},CrashCymbal);
  c3.setContents(std::vector<bool>{false,false,true,false,false,false,true,false},CrashCymbal);
  c4.setContents(std::vector<bool>{false,true,false,false,false,true,false,false},CrashCymbal);
  c5.setContents(std::vector<bool>{true,false,false,false,false,false,false,false},CrashCymbal);
  mc.contents.push_back(c1);
  mc.contents.push_back(c2);
  mc.contents.push_back(c3);
  mc.contents.push_back(c4);
  mc.contents.push_back(c5);

  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.2f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.4f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.1f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.1f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.2f));

  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.2f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.3f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.1f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.1f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.3f));

  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.1f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.3f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.1f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.3f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.2f));

  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.1f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.3f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.3f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.1f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.2f));

  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.1f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.3f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.2f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.1f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.3f));

}

void melodyRhythmInitialize(MarkovChain& mc) {
  mc.instr = MelodyRhythm;
  MarkovNode h1(8);
  MarkovNode h2(8);
  MarkovNode h3(8);
  MarkovNode h4(8);
  MarkovNode h5(8);
  MarkovNode h6(8);
  MarkovNode h7(8);
  MarkovNode h8(8);
  MarkovNode h9(8);
  MarkovNode ha(8);
  MarkovNode hb(8);
  MarkovNode hc(8);
  h1.setContents(std::vector<bool>{true,true,true,true,false,true,true,true},MelodyRhythm);
  h2.setContents(std::vector<bool>{true,false,true,true,true,true,false,true},MelodyRhythm);
  h3.setContents(std::vector<bool>{true,true,true,false,true,true,true,true},MelodyRhythm);
  h4.setContents(std::vector<bool>{true,true,true,true,false,false,true,false},MelodyRhythm);
  h5.setContents(std::vector<bool>{false,false,false,false,false,false,false,false},MelodyRhythm);
  h6.setContents(std::vector<bool>{false,true,true,true,true,false,true,true},MelodyRhythm);
  h7.setContents(std::vector<bool>{true,true,false,true,true,true,true,false},MelodyRhythm);
  h8.setContents(std::vector<bool>{false,true,true,true,false,true,false,true},MelodyRhythm);
  h9.setContents(std::vector<bool>{true,false,true,false,true,true,false,true},MelodyRhythm);
  ha.setContents(std::vector<bool>{true,true,false,true,true,false,true,true},MelodyRhythm);
  hb.setContents(std::vector<bool>{true,true,false,true,true,true,false,true},MelodyRhythm);
  hc.setContents(std::vector<bool>{true,true,true,true,true,true,true,true},MelodyRhythm);
  mc.contents.push_back(h1);
  mc.contents.push_back(h2);
  mc.contents.push_back(h3);
  mc.contents.push_back(h4);
  mc.contents.push_back(h5);
  mc.contents.push_back(h6);
  mc.contents.push_back(h7);
  mc.contents.push_back(h8);
  mc.contents.push_back(h9);
  mc.contents.push_back(ha);
  mc.contents.push_back(hb);
  mc.contents.push_back(hc);
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[0].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.25f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[1].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[2].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[3].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[4].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[5].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[6].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[7].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[8].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[9].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[10].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));

  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[0], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[1], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[2], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[3], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[4], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[5], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[6], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[7], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[8], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[9], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[10], 0.05f));
  mc.contents[11].connections.insert(std::pair<MarkovNode*, float>(&mc.contents[11], 0.05f));
}
void melodyInitialize(MelodicMarkovChain& mc) {
  MelodicMarkovNode n1(ScaleDegree::d1);
  MelodicMarkovNode n2(ScaleDegree::d5);
  MelodicMarkovNode n3(ScaleDegree::d8);
  MelodicMarkovNode n4(ScaleDegree::d9);
  mc.contents.push_back(n1);
  mc.contents.push_back(n2);
  mc.contents.push_back(n3);
  mc.contents.push_back(n4);
  mc.contents[0].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[0].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[0].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[0].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[1].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[1].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[1].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[1].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[2].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[2].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[2].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[2].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[3], 0.25f));
  mc.contents[3].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[0], 0.25f));
  mc.contents[3].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[1], 0.25f));
  mc.contents[3].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[2], 0.25f));
  mc.contents[3].connections.insert(std::pair<MelodicMarkovNode*, float>(&mc.contents[3], 0.25f));
}
*/
