#ifndef PHASEVECTOR_H_
#define PHASEVECTOR_H_
#include <gsl/gsl_rng.h>

class PhaseVector {
public:
    double phases[4];
    PhaseVector();
    PhaseVector(gsl_rng*); // Put entries to 0 by default
};

#endif // PHASEVECTOR_H_
