#pragma once

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


bool PODEM(int faultAt, int faultValue, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> inputSignals, vector<int> &outputSignals, set<int> Dfrontier);

void Objective(int faultAt, int faultValue, int &signal, int &value, vector<gateNode> &gates, vector<signalNode> &signals, set<int> Dfrontier);

void Backtrace(int &signal, int &value, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> inputSignals);

void Imply(int signal, int value, vector<gateNode> &gates, vector<signalNode> &signals, set<int> &Dfrontier);

void initialiseSignals(vector<signalNode> &signals);

void addToDFrontier(vector<gateNode> &gates, vector<signalNode> &signals, set<int> &Dfrontier); 

vector<string> printTestVector(vector<gateNode> gates, vector<signalNode> signals, vector<int> inputSignals);