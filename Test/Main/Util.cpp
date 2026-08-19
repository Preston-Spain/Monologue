#include <iostream>
#include <list>
#include <string>
#include <cmath>
#include <vector>
#include <string>


using namespace std;

class Util {
public:
    const string CSVfileDir = "D:\\Project\\Monologue\\Test\\Main\\csv"; // D:\\Project\\Monologue\\Test\\Main\\csv
    const string CSVfileActivationPoint = "ActivationPoint.csv"; // ActivationPoint.csv
    const string CSVfileConnection = "Connection.csv"; // Connection.csv
    const string CSVfileNeuron = "Neuron.csv"; // Neuron.csv
    const string CSVfileSpikingNeuron = "SpikingNeuron.csv"; // SpikingNeuron.csv

    vector<string> splitCSV(const string& line) {
        vector<string> parts;
        string token;
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            parts.push_back(token);
        }
        return parts;
    }

    vector<string> splitSpecial(const string& line, char special) {
        vector<string> parts;
        string token;
        stringstream ss(line);
        while (getline(ss, token, special)) {
            parts.push_back(token);
        }
        return parts;
    }
};

    /*
    
    */
