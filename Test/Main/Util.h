#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <random>

extern bool runAI;

class Util {
public:
    const std::string CSVfileDir;
    const std::string CSVfileActivationPoint;
    const std::string CSVfileConnection;
    const std::string CSVfileNeuron;
    const std::string CSVfileSpikingNeuron;

    bool runAI;
    std::mt19937 rng;

    Util(); // constructor declared, not defined here

    std::vector<std::string> splitCSV(const std::string& line);
    std::vector<std::string> splitSpecial(const std::string& line, char special);
    float randomFloat(float lo, float hi);
};

#endif