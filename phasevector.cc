#include "phasevector.hh"
#include <chrono>
#include <ctime>
#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <random>

const double PI = 3.141592653589793;

using namespace std;

PhaseVector::PhaseVector(gsl_rng* rng)
{
    for (int i = 0; i < 4; i++) {
        phases[i] = gsl_ran_flat(rng, 0, 2 * M_PI);
    }
}

PhaseVector::PhaseVector()
{
    for (int i = 0; i < 4; i++) {
        phases[i] = 0;
    }
}
