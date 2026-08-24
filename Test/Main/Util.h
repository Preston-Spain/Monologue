#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

extern bool runAI;

class Util {
public:
    const std::string CSVfileDir;
    const std::string CSVfileActivationPoint;
    const std::string CSVfileConnection;
    const std::string CSVfileNeuron;
    const std::string CSVfileSpikingNeuron;

    bool runAI;

    Util(); // constructor declared, not defined here

    std::vector<std::string> splitCSV(const std::string& line);
    std::vector<std::string> splitSpecial(const std::string& line, char special);};

#endif