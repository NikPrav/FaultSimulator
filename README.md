# Fault Simulator

To use the fault simulator, first compile the code via G++ compiler


# Compiling the code

Compile the code using the command
`g++ *.cpp -o out`

The simulator can be used in two ways

## 1) Run PODEM

To run PODEM, you need the following files

1) A Netlist file
2) A file with all the faults to be simulated

PODEM can be run using the following parameters

`\out "path\to\netlist" 1 "path\to\fault_file"`

eg:

`./out files/s298f_2.txt 1 faults/s298f_2.txt`