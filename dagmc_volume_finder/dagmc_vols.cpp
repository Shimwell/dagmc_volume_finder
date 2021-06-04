

#include <iostream>
#include <memory>
#include <fstream>
#include <string>

#include "DagMC.hpp"

void parse_args(int argc, char** argv, bool& verbose, int& filename_idx);

int main(int argc, char** argv) {

    bool verbose = false;
    int filename_idx = -1;

    // bar bones argument handling
    parse_args(argc, argv, verbose, filename_idx);

    // create a DAGMC instance
    std::shared_ptr<moab::DagMC> dagmc = std::make_shared<moab::DagMC>();

    moab::ErrorCode rval;

    // load the file
    rval = dagmc->load_file(argv[filename_idx]);
    MB_CHK_SET_ERR(rval, "Failed to load the file");

    // setup dagmc incideds (necessary for index loop and volume info)
    rval = dagmc->setup_indices();
    MB_CHK_SET_ERR(rval, "Failed to setup DAGMC indices");

    // needed to setup internal material data structures
    rval = dagmc->parse_properties({"mat"}, {}, ":"); // change last arg to "_" for old file formats
    MB_CHK_SET_ERR(rval, "Failed to parse material properties");

    // open CSV file and write header
    std::ofstream of("dagmc_vols.tsv");
    of << "id\tmaterial tag\tvolume (cc)\n" << std::endl;

    // loop over all volumes and get necessary info
    int n_volumes = dagmc->num_entities(3);
    for (int idx = 1; idx <= n_volumes; idx++) {
        moab::EntityHandle vol = dagmc->entity_by_index(3, idx); // DAGMC indices start at one

        // get the volume ID
        int id = dagmc->id_by_index(3, idx);

        // get the volume material assignment
        std::string mat_val;

        // measure_volume is not accurate for the implicit complement
        if (dagmc->is_implicit_complement(vol)) {
            continue;
        } else {
            rval = dagmc->prop_value(vol, "mat", mat_val);
            MB_CHK_SET_ERR(rval, "Failed to find a material assignment");
        }

        // calculate the volume
        double volume;
        rval = dagmc->measure_volume(vol, volume);
        MB_CHK_SET_ERR(rval, "Failed to measure a volume");

        // print to screen if requested
        if (verbose) {
            std::cout << "Volume " << id << " (" << mat_val << "): " << volume << std::endl;
        }

        // write volume information to the TSV file
        of << id << "\t" << mat_val << "\t" << volume << "\n";
    }

    of.close();
    return 0;
}

void parse_args(int argc, char** argv, bool& verbose, int& filename_idx) {
  bool print_help = false;

  if (argc < 1) {
   print_help = true;
  }

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-v") verbose = true;
        else if (std::string(argv[i]) == "-h") print_help = true;
        else filename_idx = i;
    }

    if (filename_idx < 0) {
        std::cout << "Error: No DAGMC file provided" << std::endl;
        print_help = true;
    }

    if (print_help) {
        std::cout << "A program for determining DAGMC volumes. \n"
                     "Usage: dagmc_vols [options] <dagmc_filename> \n"
                     "Options: \n"
                     "\t -h - print description and usage \n"
                     "\t -v - verbose mode (write into to screen)";
        std::exit(0);
    }
}
