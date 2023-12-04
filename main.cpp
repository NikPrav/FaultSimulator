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

string netlistFile;

string inputFile;
string outputFile;

int isRandom;
int isFaultFile;
int isPodem;
int checkAfterPodem;

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

void faultSimRun(vector<gateNode> gates, vector<signalNode> signals, vector<string> inputString, vector<int> inputSignals, vector<int> outputSignals, set<string> totalFaults, string faultAt = "")
{
    // iterating through all values of inputs
    vector<int> gateStack, definedSignals;
    set<string> coveredFaults;
    set<string> totalCoveredFaults;
    vector<float> coverageValues;

    vector<string> outputString;

    list<int> validList;

    int totalInputs = 0;

    bool completeCoverage = false;
    bool faultCoverage = true;
    // Populate valid list
    if (isRandom && !isPodem)
    {
        totalInputs = pow(2, inputSignals.size());
        createValidInputsList(validList, totalInputs);
    }

    for (int i = 0; i < inputString.size() || i < totalInputs; ++i)
    {
        // Clear all faults
        clearAllFaults(signals);

        string input;

        // generate inputString from validList via isRandom
        if (isRandom && !isPodem)
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

        // If running after PODEM, check if fault has been covered by input
        if (isPodem)
        {
            string x = faultAt;
            string sa = to_string(x.back() - '0');
            x.pop_back();
            if (coveredFaults.find(faultAt) != coveredFaults.end())
            {
                // cout << input << " covers " << stoi(x) + 1 << " stuck at " << sa << endl;
            }
            else
            {
                faultCoverage = false;
                // cout << input << " does not cover " << stoi(x) + 1 << " stuck at " << sa << endl;
            }
        }

        // break program if coverage is 100 percent

        // Check what all faults in defined faults are covered
        // set_intersection(coveredFaults.begin(), coveredFaults.end(), totalFaults.begin(), totalFaults.end(), inserter(validFaults, validFaults.end()));

        if (!isRandom && !isPodem)
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

        if (isRandom && !isPodem)
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
    if (!isPodem)
        std::cout << "Coverage Percentage:" << coverage << "\n";

    if (isRandom && !isPodem)
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
    else if (!isPodem)
    {
        std::cout << "Saving Output to file:" << outputFile << "\n";
        writeOutputFile(outputFile, outputString);
    }

    if (!faultCoverage)
        cout << "Some input vectors unable to cover faults";
    else
        cout << "All input vectors cover fault";
}

int main(int argc, char *argv[])
{

    netlistFile = argv[1];
    isPodem = stoi(argv[2]);
    isRandom = 0;
    isFaultFile = 0;
    string faultFile;
    checkAfterPodem = 0;

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
        if (argc > 4)
            checkAfterPodem = stoi(argv[4]);
        // isRandom = stoi(argv[5]);
        // isFaultFile = stoi(argv[6]);
    }

    if (isFaultFile)
        faultFile = argv[7];

    std::cout << "Reading file: " << netlistFile << "\n";

    vector<signalNode> signals;
    vector<gateNode> gates;
    vector<string> inputString;

    vector<int> inputSignals, outputSignals;
    vector<int> definedSignals;

    vector<string> outputString;

    // Read Netlist
    // ifstream netlist(inputFile);

    // Reading Netlist
    readNetlist(netlistFile, gates, signals, inputSignals, outputSignals);

    std::sort(signals.begin(), signals.end(), less_than_key_signalNode());

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
                vector<string> testInputs;
                testInputs = printTestVector(gates, signals, inputSignals);

                if (checkAfterPodem)
                    faultSimRun(gates, signals, testInputs, inputSignals, outputSignals, totalFaults, *it);
            }
            else
            {
                cout << "For fault " << stoi(fault) + 1 << " stuck at " << sa << " test vector is not present \n";
            }
        }

        // return (0);
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

    // faultAt = "00";

    if (!isPodem)
    {
        faultSimRun(gates, signals, inputString, inputSignals, outputSignals, totalFaults);
    }

    std::cout << "Program Exiting...";
}
