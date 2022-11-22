#ifndef MARKOV_H
#define MARKOV_H
//markov chain class definitions
class MarkovNode {
 public:
  Pattern pattern;
  std::map<MarkovNode*, float> connections;
  MarkovNode* selectNextNode();
  void setContents(std::vector<bool>, Instrument);
  MarkovNode(int n) {pattern.length = n;}
};
void MarkovNode::setContents(std::vector<bool> n, Instrument inst) {
  pattern.beats.resize(n.size());
  for(size_t i=0;i<n.size();i++) {
    pattern.beats[i].sounds.resize(NUM_OF_INSTRUMENTS);
    pattern.beats[i].sounds[inst] = n[i];
  }
}
MarkovNode* MarkovNode::selectNextNode() {
  static std::mt19937 mt(time(0));
  std::uniform_real_distribution<float> dist(0.0f,1.0f);
  float accum = 0;
  float value = dist(mt);  //randomly chosen value
  std::vector<float> ranges;   //used for choosing which node to go to
  for(auto x : connections) {
    accum += x.second;
    if(accum >= value)
      return x.first;
  }
  return this;
}
class MarkovChain {
 public:
  //used for randomly generating sequences of beats
  std::vector<MarkovNode> contents;
  MarkovNode* currentNode;
};
//initialization
#endif
