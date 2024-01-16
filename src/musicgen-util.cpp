#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <map>
#include <iostream>
#include <SFML/Audio.hpp>
#include "music-core.h"
#include "markov.h"
#include "miscutil.h"
#include "musicgen-util.h"
void generateSection(ChainList& cl, Section& sec, const int& mesToGenerate, int preMeasures, const int& mesLength) {
  auto lGetNextNode = [](MarkovChain* n){
    n->selectNextNode();
  };
  auto lMergeCurrentNode = [&](MarkovChain* n){
    sec.contents.back().merge(n->getCurrentNode()->pattern, n->instr, 8);
  };
  for(int i=0;i<mesToGenerate;i++) {
    //using current state, select next state
    std::for_each(cl.contents.begin(),cl.contents.end(),lGetNextNode);
    //append selected measure to Section
    if(preMeasures == 0) {
      sec.contents.push_back(Pattern(8));
      std::for_each(cl.contents.begin(),cl.contents.end(),lMergeCurrentNode);
    }
    else {
      i--;
      preMeasures--;
    }
  }
}

void generateMelody(MelodicMarkovChain& chain, const std::vector<bool>& rhythm, std::vector<ScaleDegree::ScaleDegree>& melody) {
  for(size_t i=0;i<rhythm.size();i++) {
    if(rhythm.at(i)) {
      chain.selectNextNode();
      melody.at(i) = chain.getCurrentNode()->note;
      //generate next scale degree using markov chain
    }
    else {
      melody.at(i) = ScaleDegree::Null;
    }
  }
}

void resolveKey(const std::vector<ScaleDegree::ScaleDegree>& im, std::vector<Note::Note>& fm, Note::Note root) {
  for(size_t i=0;i<im.size();i++) {
    fm[i] = static_cast<Note::Note>((im[i]+root)%12);
  }
}
