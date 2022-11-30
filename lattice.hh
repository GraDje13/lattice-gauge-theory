#include "phasevector.hh"
#include <vector>

using namespace std;

#ifndef LATTICE_H_
#define LATTICE_H_

class Lattice {
private:
    vector<vector<vector<vector<PhaseVector>>>> lattice; // store the phase vectors in 4 dimension lattice
    const int width; // store the width to be used as modulus in index operations.

public:
    Lattice(int, bool unordered = false);
    double average_action(); // Function to compute the average action per plaquette
};

#endif // LATTICE_H_
