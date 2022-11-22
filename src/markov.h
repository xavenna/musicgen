#ifndef MARKOV_H
#define MARKOV_H
#include <map>
#include <string>
#include <SFML/System.hpp>
//markov chain class definitions

// DUALLY_PREDICTIVE CHAINS OVERHAUL

typedef std::map<std::size_t, std::map<std::size_t, std::map<int, float>>> PredictiveMap;

class MarkovNode {
public:
  Pattern pattern;
  void setContents(std::vector<bool>, Instrument);
  MarkovNode(int);
};

class MelodicMarkovNode {  //this may need to be rewritten (so next uses last two, or use full measures) for better generation
public:
  ScaleDegree::ScaleDegree note;
  void setContents(ScaleDegree::ScaleDegree);
  MelodicMarkovNode(ScaleDegree::ScaleDegree);
  MelodicMarkovNode(int);
};

class MarkovChain {
public:
  //used for randomly generating sequences of beats
  std::vector<MarkovNode> contents;
  PredictiveMap map;
  size_t current;
  sf::Vector2i hist;
  Instrument instr;
  size_t getCurrent() const;
  MarkovNode* getCurrentNode();
  void selectNextNode();
  void setCurrentNode(size_t);
  void initialize(std::string, Instrument);
};

class MelodicMarkovChain {
public: /*
  std::vector<MelodicMarkovNode> contents;
  MelodicMarkovNode* currentNode;
  MelodicMarkovNode* getCurrentNode() const;
  void selectNextNode();
  void setCurrentNode(int);
  */
  std::vector<MelodicMarkovNode> contents;
  PredictiveMap map;
  size_t current;
  sf::Vector2i hist;
  Instrument instr;
  size_t getCurrent() const;
  MelodicMarkovNode* getCurrentNode();
  void selectNextNode();
  void setCurrentNode(size_t);
  void initialize(const std::string);
};

class ChainList {
public:
  std::vector<MarkovChain*> contents;
  MarkovChain& getChain(int);
  ChainList& append(MarkovChain*);
};

// Old definitions
/*
class MarkovNode {
public:
  Pattern pattern;
  std::map<MarkovNode*, float> connections;
  void setContents(std::vector<bool>, Instrument);
  MarkovNode(int n) {pattern.length = n;}
};
class MelodicMarkovNode {  //this may need to be rewritten (so next uses last two, or use full measures) for better generation
public:
  ScaleDegree::ScaleDegree note;
  std::map<MelodicMarkovNode*, float> connections;
  void setContents(ScaleDegree::ScaleDegree);
  MelodicMarkovNode(ScaleDegree::ScaleDegree);
};
class MarkovChain {
public:
  //used for randomly generating sequences of beats
  std::vector<MarkovNode> contents;
  MarkovNode* currentNode;
  Instrument instr;
  MarkovNode* getCurrentNode() const;
  void selectNextNode();
  void setCurrentNode(int);
};
class MelodicMarkovChain {
public:
  std::vector<MelodicMarkovNode> contents;
  MelodicMarkovNode* currentNode;
  MelodicMarkovNode* getCurrentNode() const;
  void selectNextNode();
  void setCurrentNode(int);
};
class ChainList {
public:
  std::vector<MarkovChain*> contents;
  MarkovChain& getChain(int);
  ChainList& append(MarkovChain*);
};
*/
//initialization
void hiHatInitialize(MarkovChain&);
void snareDrumInitialize(MarkovChain&);
void bassDrumInitialize(MarkovChain&);
void crashCymbalInitialize(MarkovChain&);
void rideCymbalInitialize(MarkovChain&);
void openHiHatInitialize(MarkovChain&);
void melodyRhythmInitialize(MarkovChain&);
void melodyInitialize(MelodicMarkovChain&);
#endif
