
DAGMC_DIR=/home/jshim/DAGMC
MOAB_DIR=/home/jshim/MOAB


DAGMC_LIB_DIR=$(DAGMC_DIR)/lib
MOAB_LIB_DIR=$(MOAB_DIR)/lib

CXX=g++

all:
	g++ dagmc_vols.cpp -I/usr/include/eigen3 -I$(DAGMC_DIR)/include -I$(MOAB_DIR)/include -L$(DAGMC_LIB_DIR) -L$(MOAB_LIB_DIR) -ldagmc -lMOAB -Wl,-rpath=$(DAGMC_LIB_DIR) -Wl,-rpath=$(MOAB_LIB_DIR) -o dagmc_vols

clean:
	-rm -f dagmc_vols