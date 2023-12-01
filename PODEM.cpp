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

void Objective(int faultAt, int faultValue, int &signal, int &value, vector<gateNode> &gates, vector<signalNode> &signals, set<int> Dfrontier)
{
    if (signals[faultAt].op == logicX)
    {
        signal = faultAt;
        value = logicNOT[faultValue];
        return;
    }

    for (auto iter : Dfrontier)
    {
        if (signals[gates[iter].ip1].op == logicX)
        {
            signal = gates[iter].ip1;
            value = !logicControl[gates[iter].gateType];
            return;
        }
        else if (signals[gates[iter].ip2].op == logicX)
        {
            signal = gates[iter].ip2;
            value = !logicControl[gates[iter].gateType];
            return;
        }
    }
}

void Backtrace(int &signal, int &value, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> inputSignals)
{
    bool isOutput = true;
    while (isOutput)
    {
        isOutput = false;
        for (auto gate : gates)
        {
            if (gate.op == signal)
            {
                if (signals[gate.ip1].op == logicX)
                {
                    value = value ^ logicInversion[gate.gateType];
                    signal = gate.ip1;
                    isOutput = true;
                    break;
                }
                else if (signals[gate.ip2].op == logicX)
                {
                    value = value ^ logicInversion[gate.gateType];
                    signal = gate.ip2;
                    isOutput = true;
                }
            }
        }
    }
    return;
}

void initialiseSignals(vector<signalNode> &signals)
{
    for (auto signal : signals)
    {
        signal.op = logicX;
    }
}

void addToDFrontier(vector<gateNode> &gates, vector<signalNode> &signals, set<int> &Dfrontier)
{
    int i = 0;
    // Check if any gate is no longer a D-Frontier
    for (auto it : Dfrontier)
    {
        if (signals[gates[i].op].op != logicX)
        {
            Dfrontier.erase(it);
        }
    }

    // Add new gates to DFrontier
    for (auto gate : gates)
    {
        if (signals[gate.op].op == logicX)
        {
            if (signals[gate.ip1].op == logicD || signals[gate.ip2].op == logicD || signals[gate.ip1].op == logicDbar || signals[gate.ip2].op == logicDbar)
            {
                Dfrontier.insert(i);
            }
        }
        i++;
    }
}

void evaluateGatePODEM(gateNode gate, vector<signalNode> &signals, int faultAt, int faultValue)
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

    if (gate.op == faultAt)
    {
        if (signals[gate.op].op == logicHigh && faultValue == logicLow)
        {
            signals[gate.op].op = logicD;
        }
        else if (signals[gate.op].op == logicLow && faultValue == logicHigh)
        {
            signals[gate.op].op = logicDbar;
        }
    }
}

void addToStackPODEM(vector<int> &inputSignals, vector<int> &gateStack, vector<gateNode> gates, vector<int> &evaluatedGates)
{
    // Going through all gates
    for (int i = 0; i < gates.size(); ++i)
    {
        // bool inputsDefined = false;

        // Ensure gate has not been added to stack previously
        if (find(gateStack.begin(), gateStack.end(), i) != gateStack.end())
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
            auto iter1 = std::find(inputSignals.begin(), inputSignals.end(), gates[i].ip1);
            auto iter2 = std::find(inputSignals.begin(), inputSignals.end(), gates[i].ip2); 
            if (( iter1 != inputSignals.end()))
            {

                gateStack.push_back(i);
                // inputSignals.erase(iter1);
                // adding to evaluated list to ensure it is not added to stack again
                // evaluatedGates.push_back(i);
            }
            else if (iter2 != inputSignals.end())
            {
                gateStack.push_back(i);
                // inputSignals.erase(iter2);
            }
        }
    }
}

void Imply(int signal, int value, vector<gateNode> &gates, vector<signalNode> &signals, set<int> &Dfrontier, int faultAt, int faultValue)
{
    vector<int> definedSignals;
    string input = to_string(value);
    vector<int> inputSignals(1, signal);
    set<string> totalFaults;

    vector<int> gateStack, evaluatedGates;

    definedSignals = inputSignals;

    // Initialsiing signals with inputs
    assignValues(input, signals, inputSignals, totalFaults);

    // Pushing gate to stack whose inputs are defined
    addToStackPODEM(definedSignals, gateStack, gates, evaluatedGates);

    // Evaluating the value of gates in stack until it is empty
    while (!gateStack.empty())
    {
        int currentGate = gateStack.back();

        // Evaluating the current gate on top of stack
        evaluateGatePODEM(gates[currentGate], signals, faultAt, faultValue);

        // // Evaluating fault listof output of current gate
        // evaluateFault(gates[currentGate], signals, totalFaults);

        // Clear definedSignals
        definedSignals.clear();

        // adding the output signals to the list of defined signals
        definedSignals.push_back(gates[currentGate].op);

        // Popping the Stack
        gateStack.pop_back();

        // Addding new gates whose inputs are now defined to stack
        addToStackPODEM(definedSignals, gateStack, gates, evaluatedGates);
    }

    // Checking for DFrontier
    addToDFrontier(gates, signals, Dfrontier);
}

bool PODEM(int faultAt, int faultValue, vector<gateNode> &gates, vector<signalNode> &signals, vector<int> inputSignals, vector<int> &outputSignals, set<int> Dfrontier)
{
    int k, v_k;
    int j, v_j;

    bool fail = true;

    for (auto opSignals : outputSignals)
    {
        if (signals[opSignals].op == logicD)
        {
            // Podem Success, fault propogated to output
            return true;
        }
        else if (signals[opSignals].op == logicX)
        {
            fail = false;
            // break;
        }
    }

    // All outputs have values assigned to them, fault cannot be propagated
    if (fail)
    {
        return false;
    }

    // // Finding value to be set
    Objective(faultAt, faultValue, k, v_k, gates, signals, Dfrontier);

    // // Tracing back to primary input
    Backtrace(k, v_k, gates, signals, inputSignals);

    // // Propagating input after setting value
    Imply(k, v_k, gates, signals, Dfrontier, faultAt, faultValue);

    // Call PODEM
    if (PODEM(faultAt, faultValue, gates, signals, inputSignals, outputSignals, Dfrontier))
        return true;

    // // Podem Fail, Imply opposite value
    Imply(k, logicNOT[v_k], gates, signals, Dfrontier, faultAt, faultValue);

    if (PODEM(faultAt, faultValue, gates, signals, inputSignals, outputSignals, Dfrontier))
        return true;

    // // PODEM Fail, set input value to x
    Imply(k, logicX, gates, signals, Dfrontier, faultAt, faultValue);

    return false;
}
