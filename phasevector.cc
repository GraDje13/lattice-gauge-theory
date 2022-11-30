#include "phasevector.hh"
#include <chrono>
#include <ctime>
#include <random>

const double PI = 3.141592653589793;

PhaseVector::PhaseVector(bool random)
{
    if (random) {
        static default_random_engine phase_generator(std::chrono::system_clock::now().time_since_epoch().count()); // These can be static, no need to generate them twice
        static uniform_real_distribution<double>
            phase_distribution(0, 2 * PI); // uniform distribution on [0,2\pi]

        for (int i = 0; i < 4; i++) {
            set_phase_unchecked(i, phase_distribution(phase_generator));
        }

    } else {
        for (int i = 0; i < 4; i++) {
            set_phase_unchecked(i, 0);
        }
    }
}

void PhaseVector::set_phase_unchecked(int index, double value)
{
    phases[index] = value;
}

void PhaseVector::set_phase_checked(int index, double value)
{
    while (value < 0) {
        value += 2 * PI;
    }

    phases[index] = std::fmod(value, 2 * PI); // make sure value is between 0 and 2*pi
}
