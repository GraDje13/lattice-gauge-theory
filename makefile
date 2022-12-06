##
# Lattice simulation
#
# @file
# @version 0.1

simulation: main.o phasevector.o lattice.o phasevector.hh lattice.hh
	g++ -o simulation main.o phasevector.o lattice.o -lgsl -lcblas -lm -lhdf5 -lhdf5_cpp -lboost_program_options

main.o : main.cc phasevector.hh
	g++ -c main.cc -O3

phasevector.o: phasevector.cc phasevector.hh
	g++ -c phasevector.cc -O3

lattice.o: lattice.cc phasevector.hh lattice.hh
	g++ -c lattice.cc -O3
# end
