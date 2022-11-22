#include <map>
#include <string>
#include <random>
#include <vector>
#include <iostream>
#include <ctime>
#include "miscutil.h"
float getRandFloat(float low, float high) {
  static std::mt19937 mt(time(0));
  std::uniform_real_distribution<float> dist(low, high);
  return dist(mt);
}
int getRandInt(int low, int high) {
  static std::mt19937 mt(time(0));
  std::uniform_int_distribution<int> dist(low, high);
  return dist(mt);
}


bool isNum(const std::string& st) {
  bool dp{false};
  bool hy{false};
  for(unsigned i=0;i<st.size();i++) {
    if(st[i] == '-') {
      if(hy)
	return false;
      hy = true;
    }
    else if(st[i] == '.') {
      if(dp)
	return false;
      dp = true;
    }
    else if(!isdigit(st[i]) && (st[i] != '-' || i != 0))
      return false;
  }
  return true;
}
void sSplit(const std::string& inputString, std::vector<std::string>& wordList, const std::string& key) {
  //splits a string into a vector
  //each entry is a string delimited by a character in key
  std::string currentWord;
  for(size_t i=0;i<inputString.size();i++) {
    if(ifMatch(inputString[i], key)) {
      if(!currentWord.empty()) {
	wordList.push_back(currentWord);
	currentWord.clear();
      }
    }
    else {
      currentWord += inputString[i];
    }
  }
  if(!currentWord.empty()) {
    wordList.push_back(currentWord);
    //fixes last word not being added
  }
}
bool ifMatch(char n, std::string key) { //returns whether n is found in key
	return (key.find(n) != std::string::npos);
}
std::string getRandNoun() {
  std::vector<std::string> nounsList = {"pizza","child","audience","permission","ability","opinion","uncle","investment","advice","homework","wood","grocery","examination","music","nature","conversation","birthday","recording","instance","version","professor","lady","thought","theory","proposal","tooth","responsibility","ad","organization","mall","complaint","data","bathroom","intention","flight","community","historian","psychology","food","student","physics","perception","charity","bedroom","accident","baseball","hat","currency","area","dealerchildhood","analysis","science","wealth","device","recommendation","trainer","church","heart","pizza","reaction","description","performance","farmer","instruction","worker","magazine","location","university","role","event","foundation","revenue","mall","software","refrigerator","atmosphere","cabinet","dirt","committee","breath","city","establishment","blood","leadership","psychology","platform","medicine","elevator","reading","artisan","recipe","power","fortune","player","charity","society","employer","homework","failure"};
  return nounsList[getRandInt(0,nounsList.size())];
}
//adjectives
std::string getRandomAdjective() {
  std::vector<std::string> adjList = {"exuberant","tightfisted","familiar","sloppy","cruel","obedient","adhesive","majestic","pastoral","reflective","tender","uneven","scared","ambitious","typical","secretive","abnormal","grumpy","stereotyped","luxuriant","cluttered","fascinated","administrative","helpless","rainy","graceful","oafish","minor","tacit","fast","smart","ultra","traditional","satisfying","organic","handsome","two","unequaled","critical","uttermost","aback","undesirable","calm","competitive","paltry","average","same","eight","exciting","heady","cute","three","mental","exuberant","aware","earthy","economic","womanly","rapid","standing","overwrought","broad","oceanic","verdant","utopian","temporary","living","seemly","ten","excellent","zesty","outstanding","overt","educational","wanting","limping","sexual","unused","spiffy","severe","obsolete","worried","wholesale","accessible","caring","aboriginal","internal","wet","powerful","ablaze","lopsided","closed","acrid","lavish","hilarious","endurable","lamentable","macabre","grey","eastern"};
  return adjList[getRandInt(0,adjList.size())];
}
std::string getSongName() {
  return getRandomAdjective() + " " + getRandNoun();
}
