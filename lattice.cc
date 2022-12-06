#include "lattice.hh"
#include <cmath>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <iostream>

using namespace std;

const int UNIT_VECTORS[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
const double ACCEPTANCE_CONSTANT = 0.2105137;

Lattice::Lattice(int size, bool random, gsl_rng* rng)
    // Initialize the lattice with ordered or unordered phases
    : width(size)
    , lattice(size, vector<vector<vector<PhaseVector>>>(size, vector<vector<PhaseVector>>(size, vector<PhaseVector>(size, PhaseVector()))))
{
    if (random) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                for (int k = 0; k < width; k++) {
                    for (int l = 0; l < width; l++) {
                        lattice[i][j][k][l] = PhaseVector(rng);
                    } // I wanted to have this in the constructor delegation, but it doesn't work
                }
            }
        }
    }
}

gsl_complex Lattice::plaquettes_without_link(int i, int j, int k, int l, int m)
{
    gsl_complex lambda_sum = gsl_complex_polar(0, 0);

    for (int n = 0; n < 4; n++) {
        if (n != m) {

            /* compute phases for lambda_1*/
            double phase1 = lattice[(i + UNIT_VECTORS[m][0]) % width][(j + UNIT_VECTORS[m][1]) % width][(k + UNIT_VECTORS[m][2]) % width][(l + UNIT_VECTORS[m][3]) % width].phases[n]; /* U_\nu(n+\hat{\mu}) */
            double phase2 = lattice[(i + UNIT_VECTORS[n][0]) % width][(j + UNIT_VECTORS[n][1]) % width][(k + UNIT_VECTORS[n][2]) % width][(l + UNIT_VECTORS[n][3]) % width].phases[m]; /* U_\mu(n+\hat(\nu)) */
            double phase3 = lattice[i][j][k][l].phases[n]; /*U_\nu(n)*/

            /* compute lambda1 and add to sum */
            gsl_complex lambda1 = gsl_complex_polar(1, phase1 - phase2 - phase3);
            lambda_sum = gsl_complex_add(lambda_sum, lambda1);

            /* compute phses for lambda2 */

            double phase4 = lattice[n_m(i - UNIT_VECTORS[n][0], width)][n_m(j - UNIT_VECTORS[n][1], width)][n_m(k - UNIT_VECTORS[n][2], width)][n_m(l - UNIT_VECTORS[n][3], width)].phases[m]; /* U_\mu(n - \hat(\nu)) */
            double phase5 = lattice[n_m(i - UNIT_VECTORS[n][0] + UNIT_VECTORS[m][0], width)][n_m(j - UNIT_VECTORS[n][1] + UNIT_VECTORS[m][1], width)][n_m(k - UNIT_VECTORS[n][2] + UNIT_VECTORS[m][2], width)][n_m(l - UNIT_VECTORS[n][3] + UNIT_VECTORS[m][3], width)].phases[n]; /* U_\nu(n - \hat{\nu} +\hat{\mu}) */
            double phase6 = lattice[n_m(i - UNIT_VECTORS[n][0], width)][n_m(j - UNIT_VECTORS[n][1], width)][n_m(k - UNIT_VECTORS[n][2], width)][n_m(l - UNIT_VECTORS[n][3], width)].phases[n]; /* U_\nu(n - \hat(\nu)) */

            gsl_complex lambda2 = gsl_complex_polar(1, -phase4 - phase5 + phase6);
            lambda_sum = gsl_complex_add(lambda_sum, lambda2);
        }
    }
    return lambda_sum;
}

double Lattice::average_action()
{
    // In 4d, there are 6 plaquettes per vertex
    int num_plaq = 6 * pow(width, 4);
    double sum = 0;

    // nested for loop represents sum over all vertices
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < width; k++) {
                for (int l = 0; l < width; l++) {

                    // cout << "i: " << i << " j: " << j << " k: " << k << " l: " << l << endl;

                    // sum over all plaquettes at given vertex
                    for (int m = 0; m < 3; m++) {
                        for (int n = m + 1; n < 4; n++) {
                            double phase1 = lattice[i][j][k][l].phases[m]; /* U_\mu (n) */
                            double phase2 = lattice[(i + UNIT_VECTORS[m][0]) % width][(j + UNIT_VECTORS[m][1]) % width][(k + UNIT_VECTORS[m][2]) % width][(l + UNIT_VECTORS[m][3]) % width].phases[n]; /* U_\nu(n+\hat{\mu}) */
                            double phase3 = lattice[(i + UNIT_VECTORS[n][0]) % width][(j + UNIT_VECTORS[n][1]) % width][(k + UNIT_VECTORS[n][2]) % width][(l + UNIT_VECTORS[n][3]) % width].phases[m]; /* U_\mu(n+\hat(\nu)) */
                            double phase4 = lattice[i][j][k][l].phases[n]; /* U_\nu (n) */

                            sum += 1 - cos(phase1 + phase2 - phase3 - phase4); /* take the complex conjugate of the last two */
                        }
                    }
                }
            }
        }
    }
    return sum / num_plaq;
}

double Lattice::sample_theta(double alpha, double beta, gsl_rng* rng)
{
    double prefactor = alpha * beta;

    while (true) {
        double sample_x = -1 + (1 / prefactor) * log(1 + (exp(2 * prefactor) - 1) * gsl_ran_flat(rng, 0, 1));
        if (gsl_ran_flat(rng, 0, 1) < acceptance_probability(sample_x, prefactor)) {
            double theta = (M_PI / 2) * (1 - sample_x);

            // perform coin flip
            if (gsl_rng_uniform_int(rng, 2) == 0) {
                theta = -theta;
            }

            return theta;
        }
    }
}

double Lattice::acceptance_probability(double x, double prefactor) // acceptance probability.
{
    return exp(prefactor * (cos((M_PI / 2) * (1 - x)) - x)) / exp(ACCEPTANCE_CONSTANT * prefactor);
}

void Lattice::heatbath_update(double beta, gsl_rng* rng)
{
    // sample a random link uniformly
    int random_i = gsl_rng_uniform_int(rng, width);
    int random_j = gsl_rng_uniform_int(rng, width);
    int random_k = gsl_rng_uniform_int(rng, width);
    int random_l = gsl_rng_uniform_int(rng, width);
    int random_m = gsl_rng_uniform_int(rng, 4);

    // compute alpha and theta_0
    gsl_complex other_plaquettes = plaquettes_without_link(random_i, random_j, random_k, random_l, random_m);
    double alpha = gsl_complex_abs(other_plaquettes);
    double theta_0 = -gsl_complex_arg(other_plaquettes);

    // sample a new theta
    double new_theta = sample_theta(alpha, beta, rng);

    // update the new value
    lattice[random_i][random_j][random_k][random_l].phases[random_m] = new_theta + theta_0;
}

int n_m(int i, int width) // the modulus operator in c++ does not handle negative numbers in a way that is appropriate for this application, this helper function solves that.
{
    if (i < 0) {
        return width - 1;
    } else {
        return i % width;
    }
}
