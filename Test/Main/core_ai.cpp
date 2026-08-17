using namespace std;
#include <list>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Util.cpp"

class StateStreamTransformer {
public:

	struct item {
		int ID{}; // Identifier
	};

	struct Output : item {
		float threshold{};
	};


    struct Input : item {
		int tickPosition{}; // position in activation cycle, 0 is inactive, 1 is innitatied, beyond that follows the activation function
    };

    struct Connection : item {
        list<int> InputNeuronID{};
        list<int> OutputNeuronID{};
        float weight{};
    };

    struct SpikingNeuron : Input {
		list<int> inputConnections;
		bool active{}; // is active or not
    };

    struct Neuron : Input, Output {
        list<int> inputConnections;
        list<int> outputConnections;
        bool active{};
    };

    struct ActivationPoint : Output {
		list<int> outConnections;
        int tickPosition{};
        bool active{};
    };


	list<ActivationPoint> ActivationPointInitalized;
	list<Connection> ConnectionsInitalized;
	list<Neuron> NeuronInitalized;
	list<SpikingNeuron> SpikingNeuronInitalized;


    list<Connection> getConnections(Neuron n) {
        list<Connection> connections;
        for (Connection connect : connections) {
            connections.push_back(connect);
        }
        return connections;
    }

    void init() {
		int indexer = 0;
		string text;
		for (string fileString : Util.CSVfileDir + Util.CSVfileActivationPoint, Util.CSVfileConnection, Util.CSVfileNeuron, Util.CSVfileSpikingNeuron) {
			fstream file(fileString);
			if (file.fail()) {
				cout << "File " + fileString + " does not exist" << endl;
			} else {
				getline (file, text);
				string split_view[] = text | std::views::split(','); // 0,0.0f,1$2$3$4

				switch (indexer) {
				case 0: // ActivationPoint
					ActivationPoint* ap;
					ap.ID = stoi(split_view[0]); // @var ID
					ap.threshold = stof(split_view[1]); // @var threshold

					ActivationPointInitalized.push_back(*ap);
					delete ap;
					break;
				case 1: // Connection
					Connection* c;
					c.ID = (split_view[0]); // @var ID
					c.weight = (split_view[1]); // @var weight
					ConnectionsInitalized.push_back(*c);
					delete c;
					break;
				case 2: // Neuron
					Neuron* n;
					n.ID = (split_view[0]); // @var ID
					n.threshold = (split_view[1]); // @var threshold
					NeuronInitalized.push_back(*n);
					delete n;
					break;
				case 3: // SpikingNeuron
					SpikingNeuron* sn;
					sn.ID = (split_view[0]); // @var ID
					SpikingNeuronInitalized.push_back(*sn);
					delete sn;
					break;
				default:
					cout << "Invalid file index" << endl;
					break;
				}
			}
			indexer++;
			file.close();
		}
    }

	void update() {
		// update the state of the neurons and activation points based on the current tick
		return;
	}

    float activationFunc(int& tickPosition) {
        // constant vars
        float tickOffset = 1.0f; // for testing
        float r = 1.04; // Magic number yay
        // multiplyer
        float mult = 2.0f; // Magic number yay
        // hump
        float hump = pow(2, (-1 * (tickPosition - tickOffset)) / r * 4.41/*magic number yay*/) - pow(2, (-1 * (tickPosition - tickOffset)) / r);
        if (hump < 0) {
            hump = 0;
        } else if (hump > 1) {
            hump = 1;
        }
        hump *= mult;
        // dip
        float dip = mult * 0.0f;

        return hump - dip;
    }

    float neuron(float input, Neuron n, int currentTick) {
        int p = activationFunc(n.tickPosition);

        if (n.tickPosition == 0) { // init
            if (input >= n.threshold) {
                n.tickPosition = 1;
                return p;
            }
        } else if (n.tickPosition > 0) {
            if (input >= n.threshold) {
                n.tickPosition = 1;
                return p;
            } else if (p < 0.1f) {
                n.tickPosition = 0;
                return p;
            } else if (p > 0.1f) {
                n.tickPosition++;
                return p;
            } else {
                n.tickPosition = 0;
                return 0.0f;
            }
        }
    }

    int runTick() {
        //
    }

    int run() {
        return 0;
    }
};

int main() {
  return 0;
}