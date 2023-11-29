#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

using namespace std;


enum gateTypes
{
    INV,
    AND,
    OR,
    NAND,
    NOR,
    NOT,
    XOR,
    XNOR,
    BUF,
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
    // vector<fault> faultList;
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

bool split(string inputString, vector<string> &tokens, char delimiter);

int gateType(string inputString);

void addToSignalList(int signal, int gateID, vector<signalNode> &signals);

void readInputFile(string inputFile, vector<string> &inputString);

void assignValues(string inputs, vector<signalNode> &signals, vector<int> inputSignals);

void addToStack(vector<int> inputSignals, vector<int> &gateStack, vector<gateNode> gates, vector<int> &evaluatedGates);

void evaluateGate(gateNode gate, vector<signalNode> &signals);

void readNetlist(string inputFile, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> &inputSignals, vector<int> &outputSignals);

void writeOutputFile(string fileName, vector<string> outputString);


