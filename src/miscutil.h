#ifndef MISCUTIL_H
#define MISCUTIL_H
#include <string>
#include <vector>
#include <random>

float getRandFloat(float, float);
int getRandInt(int, int);
bool ifMatch(char n, std::string key);
bool isNum(const std::string& st);
void sSplit(const std::string&, std::vector<std::string>&, const std::string&);
std::string getRandNoun();
std::string getRandomAdjective();
std::string getSongName();
#endif
