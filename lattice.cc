#include "lattice.hh"
#include <cmath>
#include <iostream>
using namespace std;

const int UNIT_VECTORS[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

Lattice::Lattice(int size, bool unordered)
    // Initialize the lattice with ordered or unordered phases
    : width(size)
    , lattice(size, vector<vector<vector<PhaseVector>>>(size, vector<vector<PhaseVector>>(size, vector<PhaseVector>(size, PhaseVector()))))
{

    if (unordered) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                for (int k = 0; k < width; k++) {
                    for (int l = 0; l < width; l++) {
                        lattice[i][j][k][l] = PhaseVector(true);
                    }
                }
            }
        }
    }
}

double Lattice::average_action()
{
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

PhaseVector Lattice::read_index(int i, int j, int k, int l)
{
    return lattice[i][j][k][l];
}
