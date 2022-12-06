#include "H5Cpp.h"
#include "lattice.hh"
#include "phasevector.hh"
#include <boost/program_options.hpp>
#include <chrono>
#include <cmath>
#include <ctime>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <iostream>

using namespace std;
using namespace H5;

namespace po = boost::program_options;

gsl_rng* rng;
bool new_simulation = true;
bool ordered = false;
bool unlimited = true;
double beta_val;
int measurements;
int measurement_interval;
int width;
int one_sweep;
int equilibration_time;

vector<double> action_measurements;

H5std_string SAVE_FILE_NAME;

const H5std_string PREVIOUS_STATE_FILE_NAME("previous_state.h5");
const H5std_string PREVIOUS_STATE_DATA_SET("previous_state");
const H5std_string PREVIOUS_STATE_BETA_DATA_SET("previous_state_beta");

const H5std_string ACTION_DATA_SET_NAME("action");
const H5std_string BETA_DATA_SET_NAME("beta");
const H5std_string WIDTH_DATA_SET_NAME("width");
const H5std_string MEASUREMENT_INTERVAL_DATA_SET_NAME("measurement-inteval");

const FloatType DOUBLE_DATA_TYPE(PredType::NATIVE_DOUBLE);
const IntType INT_DATA_TYPE(PredType::NATIVE_INT);

int main(int argc, char* argv[])
{

    // parse command line options

    po::options_description description("Usage");

    description.add_options()(
        "help", "produce help message")(
        "lattice-width", po::value<int>(&width), "specify the lattice width")(
        "beta", po::value<double>(&beta_val), "specify beta")(
        "resume", "resume from previous state")(
        "output-file", po::value<string>(), "set the name of the output file")(
        "measurements", po::value<int>(&measurements), "specify number of measurements")(
        "measurement-interval", po::value<int>(&measurement_interval), "specify sweeps between measurement")(
        "ordered", "start from an ordered configuration")(
        "equilibration-time", po::value<int>(&equilibration_time), "specify equilibration time in sweeps");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    // initialize the gsl random number generator using a seed depending on the system time
    rng = gsl_rng_alloc(gsl_rng_ranlxd2);
    gsl_rng_set(rng, chrono::system_clock::now().time_since_epoch().count());

    if (vm.count("help")) {
        cout << description << "\n";
        return 0;
    }

    if (vm.count("resume")) {
        new_simulation = false;
        cout << "not yet implemented" << endl;
        return -1;
    }

    if (new_simulation) {
        if (vm.count("lattice-width")) {
            cout << "lattice-width set to " << width << endl;
        } else {
            cout << "please enter a lattice width" << endl;
            return -1;
        }

        if (vm.count("beta")) {
            cout << "beta set to " << beta_val << endl;
        } else {
            cout << "please enter a beta" << endl;
            return -1;
        }

        if (vm.count("output-file")) {
            // TODO: check if file already exists, so data is not accidently overwritten
            SAVE_FILE_NAME = H5std_string(vm["output-file"].as<string>());
        } else {
            cout << "please specify an output file" << endl;
            return -1;
        }

        if (vm.count("measurement-interval")) {
            cout << "there will be " << measurement_interval << " sweeps between measurements" << endl;
        } else {
            cout << "please specify a measurement interval" << endl;
            return -1;
        }

        if (vm.count("equilibration-time")) {
            cout << "there will be " << equilibration_time << " equilibration sweeps" << endl;
        } else {
            cout << "please enter a equilibration time" << endl;
            return -1;
        }

        if (vm.count("measurements")) {
            unlimited = false;
            cout << "simulation will do " << measurements << " measurements" << endl;
        } else {
            cout << "simulation will run until heat death of the universe" << endl;
        }

        if (vm.count("ordered")) {
            ordered = true;
            cout << "simulation will start from an ordered configuration" << endl;
        } else {
            cout << "simulation will start from an unordered configuration" << endl;
        }

        one_sweep = 4 * width * width * width * width; // one sweep is equal to the number of independent links.
    }

    Lattice lattice(width, !ordered, rng);

    // equilibration phase
    for (int i = 0; i < one_sweep * equilibration_time; i++) {
        lattice.heatbath_update(beta_val, rng);
    }

    if (unlimited) {
        for (int i = 0; i < one_sweep * measurement_interval; i++) {
            lattice.heatbath_update(beta_val, rng);
        }
        action_measurements.push_back(lattice.average_action());
        // TODO: save in chunks, dont use the unlimited simulation yet because it does not save anything
    } else {
        for (int i = 0; i < measurements; i++) {
            for (int j = 0; j < one_sweep * measurement_interval; j++) {
                lattice.heatbath_update(beta_val, rng);
            }
            action_measurements.push_back(lattice.average_action());
            // TODO: save in chunks, now there is only one save at the end, which is dangerous
        }

        H5File file(SAVE_FILE_NAME, H5F_ACC_TRUNC);

        // TODO: saving metadata can be moved to the start of the simulation

        hsize_t measurements_size[1] = { hsize_t(measurements) };
        hsize_t beta_size[1] = { 1 };
        hsize_t width_size[1] = { 1 };
        hsize_t measurement_interval_size[1] = { 1 };

        DataSpace measurement_dataspace(1, measurements_size);
        DataSpace beta_dataspace(1, beta_size);
        DataSpace width_dataspace(1, width_size);
        DataSpace measurement_interval_dataspace(1, measurement_interval_size);

        DataSet action_dataset = file.createDataSet(ACTION_DATA_SET_NAME, DOUBLE_DATA_TYPE, measurement_dataspace);
        DataSet beta_dataset = file.createDataSet(BETA_DATA_SET_NAME, DOUBLE_DATA_TYPE, beta_dataspace);
        DataSet width_dataset = file.createDataSet(WIDTH_DATA_SET_NAME, INT_DATA_TYPE, width_dataspace);
        DataSet measurement_interval_dataset = file.createDataSet(MEASUREMENT_INTERVAL_DATA_SET_NAME, INT_DATA_TYPE, measurement_interval_dataspace);

        action_dataset.write(action_measurements.data(), PredType::NATIVE_DOUBLE);

        double betas[1] = { beta_val };
        beta_dataset.write(betas, PredType::NATIVE_DOUBLE);

        int widths[1] = { width };
        width_dataset.write(widths, PredType::NATIVE_INT);

        int intervals[1] = { measurement_interval };
        measurement_interval_dataset.write(intervals, PredType::NATIVE_INT);
    }

    return 0;
}
