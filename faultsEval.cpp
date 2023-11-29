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

using namespace std;

void evaluateFault(gateNode gate, vector<signalNode> &signals, set<string> totalFaults)
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

        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
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
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }

        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
            // Adding op s-a fault to list
            signals[gate.op].faultList.insert(curFault);

        break;
    case OR:
        if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
            // Adding op s-a fault to list
            signals[gate.op].faultList.insert(curFault);
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
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
            // Adding op s-a fault to list
            signals[gate.op].faultList.insert(curFault);
        break;
    case NOR:
        if (!signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
            signals[gate.op].faultList.insert(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end());
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (!signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && !signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_difference(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        else if (signals[gate.ip1].op && signals[gate.ip2].op)
        {
            // stuck at fault
            curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);
            // Unity of both input lists, along with the stuck at fault
            set_intersection(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end(), signals[gate.ip2].faultList.begin(), signals[gate.ip2].faultList.end(), inserter(signals[gate.op].faultList, signals[gate.op].faultList.end()));
            // signals[gate.op].faultList.insert(curFault);
        }
        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
            // Adding op s-a fault to list
            signals[gate.op].faultList.insert(curFault);
        break;
    case BUF:
        // Considering s-a for op signal
        curFault = to_string(signals[gate.op].signalID) + to_string(!signals[gate.op].op);

        // Adding ip list
        signals[gate.op].faultList.insert(signals[gate.ip1].faultList.begin(), signals[gate.ip1].faultList.end());
        // Checking is curFault is in faultList
        if (totalFaults.find(curFault) != totalFaults.end())
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

void readFaultlist(string inputFile, set<string> &totalFaults)
{
    ifstream file(inputFile);
    // vector<string> tokens;

    while (file)
    {
        string line;
        string fault;
        getline(file, line);
        vector<string> tokens;

        if (line.size() == 0)
        {
            break;
        }

        // Splitting into tokens
        split(line, tokens, ' ');

        // // Remove last character to stop it bugging out in linux
        // line.pop_back();

        fault += to_string(stoi(tokens[0]) - 1);
        fault += tokens[1];

        totalFaults.insert(fault);
    }
    file.close();
}