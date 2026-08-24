#include "Util.h"
#include <iostream>
#include <sstream>

using namespace std;

Util::Util()
    : CSVfileDir("D:\\Project\\Monologue\\Test\\Main\\csv"),
      CSVfileActivationPoint("ActivationPoint.csv"),
      CSVfileConnection("Connection.csv"),
      CSVfileNeuron("Neuron.csv"),
      CSVfileSpikingNeuron("SpikingNeuron.csv")
{}

vector<string> Util::splitCSV(const string& line) {
    vector<string> parts;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ',')) parts.push_back(token);
    return parts;
}

vector<string> Util::splitSpecial(const string& line, char special) {
    vector<string> parts;
    string token;
    stringstream ss(line);
    while (getline(ss, token, special)) parts.push_back(token);
    return parts;
}

// bool Util::yesNoPrompt(const string& prompt) {
//     string input;
//     cout << prompt << " (y/n): ";
//     cin >> input;
//     return (input == "y" || input == "Y");
// }

bool runAI = false;