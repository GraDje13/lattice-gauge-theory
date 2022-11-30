#include <vector>

using namespace std;

#ifndef PHASEVECTOR_H_
#define PHASEVECTOR_H_

class PhaseVector {
public:
    double phases[4];
    PhaseVector(bool random = false);
    double read_phase(int) const;
    void set_phase_unchecked(int, double);
    void set_phase_checked(int, double); // This version ensures the resulting value is within [0, 2\pi]
};

#endif // PHASEVECTOR_H_
