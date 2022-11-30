##
# Lattice simulation
#
# @file
# @version 0.1

simulation: main.o phasevector.o lattice.o phasevector.hh lattice.hh
	g++ -o simulation main.o phasevector.o lattice.o

main.o : main.cc phasevector.hh
	g++ -c main.cc

phasevector.o: phasevector.cc phasevector.hh
	g++ -c phasevector.cc

lattice.o: lattice.cc phasevector.hh lattice.hh
	g++ -c lattice.cc
# end
