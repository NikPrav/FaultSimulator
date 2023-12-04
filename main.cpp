#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <list>

#include <omp.h>

#include "gateList.h"
#include "faultsEval.h"
#include "PODEM.h"

using namespace std;

void writeToFile(vector<float> coverageValues, string fileName)
{

    ofstream file(fileName);
    // file << "[ ";
    for (auto output : coverageValues)
    {

        file << output << ",";
    }
    file.close();
}

int main(int argc, char *argv[])
{
    string netlistFile = argv[1];
    string inputFile;
    string outputFile;
    int isPodem = stoi(argv[2]);
    int isRandom = 0;
    int isFaultFile = 0;
    string faultFile;

    if (!isPodem)
    {
        inputFile = argv[3];
        outputFile = argv[4];
        isRandom = stoi(argv[5]);
        isFaultFile = stoi(argv[6]);
    }
    else
    {
        cout << "isPodem\n";
        faultFile = argv[3];
        // isRandom = stoi(argv[5]);
        // isFaultFile = stoi(argv[6]);
    }

    if (isFaultFile)
        faultFile = argv[7];

    std::cout << "Reading file: " << netlistFile << "\n";

    vector<signalNode> signals;
    vector<gateNode> gates;
    vector<string> inputString;

    vector<float> coverageValues;

    vector<int> gateStack, inputSignals, outputSignals;
    vector<int> definedSignals;

    vector<string> outputString;

    // Read Netlist
    // ifstream netlist(inputFile);

    // Reading Netlist
    readNetlist(netlistFile, gates, signals, inputSignals, outputSignals);

    sort(signals.begin(), signals.end(), less_than_key_signalNode());

    definedSignals = inputSignals;

    bool podemRun = isPodem;
    set<int> Dfrontier;
    // Total Possible faults
    set<string> totalFaults;
    if (podemRun)
    {
        cout << "Reading faultlists: " << faultFile << endl;
        readFaultlist(faultFile, totalFaults);
        for (set<string>::iterator it = totalFaults.begin(); it != totalFaults.end(); ++it)
        {
            string fault = *it;
            string sa = to_string(fault.back() - '0');
            fault.pop_back();
            initialiseSignals(signals);
            bool isPODEMSuccess = PODEM(stoi(fault), stoi(sa), gates, signals, inputSignals, outputSignals, Dfrontier);

            if (isPODEMSuccess)
            {
                cout << "For fault " << stoi(fault) + 1 << " stuck at " << sa << " the test vector is:";
                printTestVector(gates, signals, inputSignals);
            }
            else
            {
                cout << "For fault " << stoi(fault) + 1 << " stuck at " << sa << " test vector is not present \n";
            }
        }

        return (0);
    }

    // Reading inputs
    if (!isRandom)
    {
        std::cout << "Reading inputs:" << inputFile << "\n";
        readInputFile(inputFile, inputString);
    }

    // Keeping note of faults covered
    set<string> coveredFaults;
    // set<string> validFaults;
    // set<string> totalValidFaults;

    if (isFaultFile)
        readFaultlist(faultFile, totalFaults);
    else
        findTotalFaults(totalFaults, signals);

    set<string> totalCoveredFaults;

    list<int> validList;

    int totalInputs = 0;
    // Populate valid list
    if (isRandom)
    {
        totalInputs = pow(2, inputSignals.size());
        createValidInputsList(validList, totalInputs);
    }

    bool completeCoverage = false;

    // iterating through all values of inputs
    for (int i = 0; i < inputString.size() || i < totalInputs; ++i)
    {
        // Clear all faults
        clearAllFaults(signals);

        string input;

        // generate inputString from validList via isRandom
        if (isRandom)
        {
            input = generateRandomInput(inputSignals, validList);
            std::cout << "Generating input " << i << " out of " << totalInputs << "\n";
        }
        else
        {
            input = inputString[i];
        }

        definedSignals = inputSignals;
        vector<int> evaluatedGates;
        // Initialising input signals with inputs
        assignValues(input, signals, inputSignals, totalFaults);

        // Pushing gate to stack whose inputs are defined
        addToStack(definedSignals, gateStack, gates, evaluatedGates);

        // Evaluating the value of gates in stack until it is empty
        while (!gateStack.empty())
        {
            int currentGate = gateStack.back();

            // Evaluating the current gate on top of stack
            evaluateGate(gates[currentGate], signals);

            // Evaluating fault listof output of current gate
            evaluateFault(gates[currentGate], signals, totalFaults);

            // adding the output signals to the list of defined signals
            definedSignals.push_back(gates[currentGate].op);

            // Popping the Stack
            gateStack.pop_back();

            // Addding new gates whose inputs are now defined to stack
            addToStack(definedSignals, gateStack, gates, evaluatedGates);
        }

        // Saving output signal list to string
        string output;

        for (int signal : outputSignals)
        {
            output = output + to_string(signals[signal].op);

            // Aggregate outputs
            coveredFaults.insert(signals[signal].faultList.begin(), signals[signal].faultList.end());
        }

        // break program if coverage is 100 percent

        // Check what all faults in defined faults are covered
        // set_intersection(coveredFaults.begin(), coveredFaults.end(), totalFaults.begin(), totalFaults.end(), inserter(validFaults, validFaults.end()));

        if (!isRandom)
        {
            std::cout << "For input " << inputString[i] << "\n";
            std::cout << "###### Faults Covered ####### \n";
            // Print Faults covered by input
            printFaults(coveredFaults);
        }
        // Compare faults to total number of faults and calcuate coverage
        totalCoveredFaults.insert(coveredFaults.begin(), coveredFaults.end());
        outputString.push_back(output);
        coveredFaults.clear();

        if (isRandom)
        {
            //  validFaults is intersection of converedFaults and totalFaults

            // list validFaults =
            // set_intersection(totalCoveredFaults.begin(), totalCoveredFaults.end(), totalFaults.begin(), totalFaults.end(), inserter(validFaults, validFaults.end()));
            float coverage = totalCoveredFaults.size() * 100.0 / totalFaults.size();
            std::cout << "Coverage percentage:" << coverage << "\n";
            coverageValues.push_back(coverage);
            if (coverage >= 90)
            {
                std::cout << "The design has reached 90 percent coverage after " << i + 1 << " random inputs!! \n";
                completeCoverage = true;
                writeToFile(coverageValues, "coverage.txt");
                break;
            }
        }
    }

    // set_intersection(totalCoveredFaults.begin(), totalCoveredFaults.end(), totalFaults.begin(), totalFaults.end(), inserter(validFaults, validFaults.end()));

    float coverage = totalCoveredFaults.size() * 100.0 / totalFaults.size();

    std::cout << "Coverage Percentage:" << coverage << "\n";

    if (isRandom)
    {
        if (!completeCoverage)
        {
            std::cout << "The design has unreachable faults \n";
            set<string> unreachableFaults;
            set_difference(totalFaults.begin(), totalFaults.end(), totalCoveredFaults.begin(), totalCoveredFaults.end(), inserter(unreachableFaults, unreachableFaults.end()));
            ;
            printFaults(unreachableFaults);
        }
    }
    else
    {
        std::cout << "Saving Output to file:" << outputFile << "\n";
        writeOutputFile(outputFile, outputString);
    }

    std::cout << "Program Exiting...";
}
