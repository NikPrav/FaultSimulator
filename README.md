# Fault Simulator

To use the fault simulator, first compile the code via G++ compiler


## Compiling the code

Compile the code using the command:

`g++ *.cpp -o out`

The command line args are:

`/out "path/to/netlist" isPodemFlag "path/to/inputs" "path/to/store/results" isRandomFlag isFaultFileFlag "path/to/fault_file"`

The simulator can be used in two ways

## 1) Run PODEM

To run PODEM, you need the following files

1) A Netlist file
2) A file with all the faults to be simulated

PODEM can be run using the following parameters. The important thing to note here is to set the isPodem Flag to 1.

`/out "path/to/netlist" 1 "path/to/fault_file"`

eg:

`./out files/s298f_2.txt 1 faults/s298f_2.txt`

To verify if all tests cover the give fault, an additional flag can be added at the end, checkAfterPodem

`./out files/s298f_2.txt 1 faults/s298f_2.txt 1`

This will go through all possible test vectors, and verify that the fault is indeed caught by the simulator

For an explanation of the flags and parameters, please see below

## 2) Run Deductive Fault Simulation

To run Deductive fault simulation, you need the following

1) A Netlist file
2) A file with the inputs to be simulated

Optionally, by setting some flags, you can also include the following

3) A file with the faults to be considered. If flag isFaultFile is set, only faults from this file are cosidered

To run simulation, the following command can be used

`/out "path/to/netlist" 0 "path/to/inputs" "path/to/store/results" isRandomFlag isFaultFileFlag "path/to/fault_file"`

eg:

`./out files/s298f_2.txt 0 inputs/s298f_2.txt output/s298f_2.txt 0 1 faults/s298f_2.txt`

To check coverage statistics, run with the isRandom flag:

`./out files/s298f_2.txt 0 inputs/s298f_2.txt output/s298f_2.txt 1 1 faults/s298f_2.txt`