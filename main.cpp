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

using namespace std;

void evaluateFault(gateNode gate, vector<signalNode> &signals)
{
    string curFault;
    switch (gate.gateType)
    {
    case INV:
    case NOT:
        // Considering s-a for op signal
        curFault = to_string(signals[gate.op].signalID) + to_string(signals[gate.ip1].op);

        // Adding ip list
        signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
        // Adding op s-a fault to list
        signals[gate.op].faultList.insert(curFault);
        break;
    case AND:

        if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }

        break;
    case OR:
        if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        break;
    case XOR:
        signals[gate.op].op = signals[gate.ip1].op ^ signals[gate.ip2].op;
        break;
    case XNOR:
        signals[gate.op].op = !(signals[gate.ip1].op ^ signals[gate.ip2].op);
        break;
    case NAND:
        if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        break;
    case NOR:
        if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            signals[gate.op].faultList.insert(curFault);
        }
        break;
    case BUF:
        // Considering s-a for op signal
        curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);

        // Adding ip list
        signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
        // Adding op s-a fault to list
        signals[gate.op].faultList.insert(curFault);
        break;
    }
}

void findTotalFaults(set<string> &totalFaults, vector<signalNode> &signals)
{
    string curFault;
    for (auto signal : signals)
    {
        curFault = to_string(signal.signalID) + "0";
        totalFaults.insert(curFault);
        curFault = to_string(signal.signalID) + "1";
        totalFaults.insert(curFault);
    }
}

void clearAllFaults(vector<signalNode> &signals)
{
    for (auto signal : signals)
    {
        signal.faultList.clear();
    }
}

void printFaults(set<string> faultList)
{
    for (set<string>::iterator it = faultList.begin(); it != faultList.end(); ++it)
    {
        string fault = *it;
        string sa = to_string(fault.back() - '0');
        fault.pop_back();

        std::cout << stoi(fault) + 1 << " stuck at " << sa << "\n";
    }
}

string generateRandomInput(vector<int> inputSignals, list<int> &validList)
{
    // Figure out what the input space is
    int inputSpace = validList.size();

    // generate a random number
    int random = rand() % inputSpace;

    auto l_front = validList.begin();

    l_front = std::next(l_front, random);

    // Select random index from valid list
    int input = *l_front;

    string inputString = std::bitset<32>(input).to_string();

    // Find the first '1' in the binary string to remove leading zeros
    size_t pos = inputString.find('1');
    if (pos != std::string::npos)
    {
        inputString = inputString.substr(pos);
    }
    else
    {
        inputString = "0"; // Handle the case when the integer is 0
    }

    validList.erase(l_front);

    return inputString;
}

void createValidInputsList(list<int> &validList, int n)
{

    for (int i = 0; i < n; ++i)
    {
        validList.push_back(i);
    }
}

void writeToFile(vector<float> coverageValues, string fileName){
    
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
    string inputFile = argv[2];
    string outputFile = argv[3];
    int isRandom = stoi(argv[4]);

    std::cout << "Reading file:" << netlistFile << "\n";

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

    // Reading inputs
    if (!isRandom)
    {
        std::cout << "Reading inputs:" << inputFile << "\n";
        readInputFile(inputFile, inputString);
    }

    // Keeping note of faults covered
    set<string> coveredFaults;

    // Total Possible faults
    set<string> totalFaults;
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
        assignValues(input, signals, inputSignals);

        // Pushing gate to stack whose inputs are defined
        addToStack(definedSignals, gateStack, gates, evaluatedGates);

        // Evaluating the value of gates in stack until it is empty
        while (!gateStack.empty())
        {
            int currentGate = gateStack.back();

            // Evaluating the current gate on top of stack
            evaluateGate(gates[currentGate], signals);

            // Evaluating fault listof output of current gate
            evaluateFault(gates[currentGate], signals);

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

    //
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
