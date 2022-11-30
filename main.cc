#include "lattice.hh"
#include "phasevector.hh"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{

    Lattice ordered_lattice(3);
    Lattice unordered_lattice(3, true);

    cout << ordered_lattice.average_action() << endl;
    cout << unordered_lattice.average_action() << endl;

    return 0;
}
