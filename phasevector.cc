#include "phasevector.hh"
#include <chrono>
#include <ctime>
#include <random>

const double PI = 3.141592653589793;

using namespace std;

PhaseVector::PhaseVector(bool random)
{
    if (random) {
        // make phase vector with random entries, useful for creating an unordered initial state.
        static default_random_engine phase_generator(chrono::system_clock::now().time_since_epoch().count()); // These can be static, no need to generate them twice
        static uniform_real_distribution<double>
            phase_distribution(0, 2 * PI); // uniform distribution on [0,2\pi]

        for (int i = 0; i < 4; i++) {
            phases[i] = phase_distribution(phase_generator);
        }

    } else {
        for (int i = 0; i < 4; i++) {
            phases[i] = 0;
        }
    }
}
