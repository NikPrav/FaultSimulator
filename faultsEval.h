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

#include "gateList.h"

void evaluateFault(gateNode gate, vector<signalNode> &signals, set<string> totalFaults);

void findTotalFaults(set<string> &totalFaults, vector<signalNode> &signals);

void clearAllFaults(vector<signalNode> &signals);

void printFaults(set<string> faultList);

string generateRandomInput(vector<int> inputSignals, list<int> &validList);

void createValidInputsList(list<int> &validList, int n);

void readFaultlist(string inputFile, set<string> &totalFaults);



