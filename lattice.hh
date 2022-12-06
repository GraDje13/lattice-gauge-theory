#include "phasevector.hh"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_rng.h>
#include <vector>
using namespace std;

#ifndef LATTICE_H_
#define LATTICE_H_

class Lattice {
private:
    vector<vector<vector<vector<PhaseVector>>>> lattice; // store the phase vectors in 4 dimension lattice
    const int width; // store the width to be used as modulus in index operations.
    gsl_complex plaquettes_without_link(int, int, int, int, int);
    double acceptance_probability(double, double);
    double sample_theta(double, double, gsl_rng*);

public:
    Lattice(int, bool, gsl_rng*);
    double average_action(); // Function to compute the average action per plaquette
    void heatbath_update(double, gsl_rng*);
};

int n_m(int, int); // modulus for when things can go negative
#endif // LATTICE_H_
