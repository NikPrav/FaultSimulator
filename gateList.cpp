#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <cstring>

// #include "PODEM.h"

using namespace std;

enum logicValues
{
    logicLow,
    logicHigh,
    logicD,
    logicDbar,
    logicX
};

struct fault
{
    int signalID;
    int fault;
};

enum gateTypes
{
    INV,
    AND,
    OR,
    NAND,
    NOR,
    BUF,
    NOT,
    XOR,
    XNOR,
    INPUT,
    OUTPUT
};

struct gateNode
{
    int ip1, ip2;
    int op;
    int gateType;
    int gateID;
    vector<int> signalList;
};

struct signalNode
{
    int signalID;
    vector<int> gateIDs;
    set<string> faultList;
    int op;
};

struct less_than_key_signalNode
{
    inline bool operator()(const signalNode &struct1, const signalNode &struct2)
    {
        return (struct1.signalID < struct2.signalID);
    }
};

int logicAND[5][5] = {{0, 0, 0, 0, 0},
                      {0, 1, 2, 3, 4},
                      {0, 2, 2, 0, 4},
                      {0, 3, 0, 3, 4},
                      {0, 4, 4, 4, 4}};

int logicOR[5][5] = {{0, 1, 2, 3, 4},
                     {1, 1, 1, 1, 1},
                     {2, 1, 2, 1, 4},
                     {3, 1, 1, 3, 4},
                     {4, 1, 4, 4, 4}};

int logicNAND[5][5] = {{1, 1, 1, 1, 1},
                       {1, 0, 3, 2, 4},
                       {1, 3, 3, 1, 4},
                       {1, 2, 1, 2, 4},
                       {1, 4, 4, 4, 4}};

int logicNOR[5][5] = {{1, 0, 3, 2, 4},
                      {0, 0, 0, 0, 0},
                      {3, 0, 3, 0, 4},
                      {2, 0, 0, 2, 4},
                      {4, 0, 4, 4, 4}};

int logicNOT[5] = {1, 0, 3, 2, 4};

int logicControl[6] = {1, 0, 1, 0, 1, 0};
int logicInversion[6] = {1, 0, 0, 1, 1, 0};

bool split(string inputString, vector<string> &tokens, char delimiter)
{
    // Check if empty
    if (inputString.empty())
    {
        return false;
    }
    else
    {
        string token;
        istringstream tokenStream(inputString);

        // iterate through all words seperated by space and add to vector
        while (getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return true;
    }
}

int gateType(string inputString)
{
    vector<string> allowedGates{"INV", "AND", "OR", "NAND", "NOR", "BUF", "NOT", "XOR", "XNOR",  "INPUT", "OUTPUT"};

    for (int i = 0; i < allowedGates.size(); ++i)
    {
        if (!allowedGates[i].compare(inputString))
        {
            return i;
        }
    }

    return -1;
}

void addToSignalList(int signal, int gateID, vector<signalNode> &signals)
{

    // search if signal exists
    // [OPTIMISABLE]
    for (int i = 0; i < signals.size(); ++i)
    {
        // if it exists, add gateID to corresponding signal
        if (signals[i].signalID == signal)
        {
            signals[i].gateIDs.push_back(gateID);
            return;
        }
    }

    // if it does not, create new signalNode and add gateID
    signalNode newSignal;
    newSignal.signalID = signal;
    newSignal.op = logicX;
    newSignal.gateIDs.push_back(gateID);

    signals.push_back(newSignal);
}

void readInputFile(string inputFile, vector<string> &inputString)
{
    ifstream file(inputFile);
    // vector<string> tokens;

    while (file)
    {
        string line;
        getline(file, line);

        if (line.size() == 0)
        {
            break;
        }
        // // Remove last character to stop it bugging out in linux
        // line.pop_back();

        inputString.push_back(line);
    }
    file.close();
}

void assignValues(string inputs, vector<signalNode> &signals, vector<int> inputSignals, set<string> &totalFaults)
{
    string fault;
    // Iterate through inputSignals list and assign values according to inputs
    for (int i = 0; i < inputSignals.size(); ++i)
    {

        // Assigning value to signal
        // for (int j = 0; j < signals.size(); ++j)
        // {
        signals[inputSignals[i]].op = inputs[i] - '0';
        fault = to_string(signals[inputSignals[i]].signalID) + to_string(!(inputs[i] - '0'));
        if (totalFaults.find(fault) != totalFaults.end())
            signals[inputSignals[i]].faultList.insert(fault);
        // }
    }
}

void addToStack(vector<int> &inputSignals, vector<int> &gateStack, vector<gateNode> gates, vector<int> &evaluatedGates)
{
    // Going through all gates
     for (int i = 0; i < gates.size(); ++i)
    {
        // bool inputsDefined = false;

        // Ensure gate has not been added to stack previously
        if (find(evaluatedGates.begin(), evaluatedGates.end(), i) != evaluatedGates.end())
        {
            continue;
        }

        // check if inputs are in inputSignals list

        switch (gates[i].gateType)
        {
        case INV:
        case BUF:
        case NOT:
            // Check for ip1
            if (find(inputSignals.begin(), inputSignals.end(), gates[i].ip1) != inputSignals.end())
            {
                gateStack.push_back(i);
                // adding to evaluated list to ensure that it is not added to stack again
                evaluatedGates.push_back(i);
            }
            break;
        default:
            // Chec for both ip1 and ip2
            if (std::find(inputSignals.begin(), inputSignals.end(), gates[i].ip1) != inputSignals.end())
            {
                if (std::find(inputSignals.begin(), inputSignals.end(), gates[i].ip2) != inputSignals.end())
                {
                    gateStack.push_back(i);
                    // adding to evaluated list to ensure it is not added to stack again
                    evaluatedGates.push_back(i);
                }
            }
        }
    }
}

void evaluateGate(gateNode gate, vector<signalNode> &signals)
{
    switch (gate.gateType)
    {
    case INV:
    case NOT:
        signals[gate.op].op = logicNOT[signals[gate.ip1].op];
        break;
    case AND:
        signals[gate.op].op = logicAND[signals[gate.ip1].op][signals[gate.ip2].op];
        break;
    case OR:
        signals[gate.op].op = logicOR[signals[gate.ip1].op][signals[gate.ip2].op];
        break;
    case XOR:
        signals[gate.op].op = signals[gate.ip1].op ^ signals[gate.ip2].op;
        break;
    case XNOR:
        signals[gate.op].op = !(signals[gate.ip1].op ^ signals[gate.ip2].op);
        break;
    case NAND:
        signals[gate.op].op = logicNAND[signals[gate.ip1].op][signals[gate.ip2].op];
        break;
    case NOR:
        signals[gate.op].op = logicNOR[signals[gate.ip1].op][signals[gate.ip2].op];
        break;
    case BUF:
        signals[gate.op].op = signals[gate.ip1].op;
        break;
    }
}

void readNetlist(string inputFile, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> &inputSignals, vector<int> &outputSignals)
{

    ifstream file(inputFile);
    vector<string> tokens;
    int GateID = 0;

    while (file)
    {
        tokens.clear();
        string line;
        int inputGate;

        gateNode gate;

        getline(file, line);

        if (line.empty() || line.compare("\r") == 0)
        {
            break;
        }

        // Removing last character from string
        // line.pop_back();

        // Splitting into tokens
        split(line, tokens, ' ');

        // Switching according to gate type
        inputGate = gateType(tokens[0]);

        gate.gateID = GateID;
        gate.gateType = inputGate;

        switch (inputGate)
        {
        case INV:
        case NOT:
        case BUF:
            gate.ip1 = stoi(tokens[1]) - 1;
            gate.op = stoi(tokens[2]) - 1;

            // Adding gates to signal list
            addToSignalList(gate.ip1, GateID, signals);
            addToSignalList(gate.op, GateID, signals);
            gates.push_back(gate);
            break;
        case INPUT:
            // Pushing inputs to inputSignals
            for (int i = 1; i < tokens.size(); i++)
            {
                if (!tokens[i].compare("-1\r") || !tokens[i].compare("-1"))
                    break;
                if (tokens[i].empty())
                    continue;
                inputSignals.push_back(stoi(tokens[i]) - 1);
            }

            break;
        case OUTPUT:
            // Pushing outputs to outputSignals
            for (int i = 1; i < tokens.size(); i++)
            {
                if (!tokens[i].compare("-1\r") || !tokens[i].compare("-1"))
                    break;
                if (tokens[i].empty())
                    continue;
                outputSignals.push_back(stoi(tokens[i]) - 1);
            }
            break;
        default:
            // Normal Excecution
            gate.ip1 = stoi(tokens[1]) - 1;
            gate.ip2 = stoi(tokens[2]) - 1;
            gate.op = stoi(tokens[3]) - 1;

            // Adding gates to signal list
            addToSignalList(gate.ip1, GateID, signals);
            addToSignalList(gate.ip2, GateID, signals);
            addToSignalList(gate.op, GateID, signals);
            gates.push_back(gate);
            break;
        }

        GateID++;
        // cout << line;
    }
    file.close();
}

void writeOutputFile(string fileName, vector<string> outputString)
{
    ofstream file(fileName);

    for (auto output : outputString)
    {
        file << output << "\n";
    }
    file.close();
}