#include "Util.h"
#include <iostream>
#include <sstream>
#include <random>

using namespace std;

Util::Util()
    : CSVfileDir("D:\\Project\\Monologue\\Test\\Main\\csv"),
      CSVfileActivationPoint("ActivationPoint.csv"),
      CSVfileConnection("Connection.csv"),
      CSVfileNeuron("Neuron.csv"),
      CSVfileSpikingNeuron("SpikingNeuron.csv"),
      rng(std::random_device{}())
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

float Util::randomFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng);
}

bool runAI = false;