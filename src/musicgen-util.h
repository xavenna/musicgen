#ifndef MUSICGEN_UTIL_H
#define MUSICGEN_UTIL_H

void generateSection(ChainList&, Section&, const int&, int, const int&);
void generateMelody(MelodicMarkovChain&, const std::vector<bool>&, std::vector<ScaleDegree::ScaleDegree>&);
void resolveKey(const std::vector<ScaleDegree::ScaleDegree>&, std::vector<Note::Note>&, Note::Note);
#endif
