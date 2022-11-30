#include "phasevector.hh"
#include <vector>

using namespace std;

#ifndef LATTICE_H_
#define LATTICE_H_

class Lattice {
private:
    vector<vector<vector<vector<PhaseVector>>>> lattice;
    const int width;

public:
    Lattice(int, bool unordered = false);
    double average_action();
    PhaseVector read_index(int, int, int, int);
};

#endif // LATTICE_H_
