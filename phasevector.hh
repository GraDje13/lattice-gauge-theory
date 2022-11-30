#ifndef PHASEVECTOR_H_
#define PHASEVECTOR_H_

class PhaseVector {
public:
    double phases[4];
    PhaseVector(bool random = false); // Put entries to 0 by default
};

#endif // PHASEVECTOR_H_
