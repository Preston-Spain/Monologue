using namespace std;
#include <list>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Util.h"

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

        // FIX: braces make this a real list of 4 filenames.
        // The comma version only ever iterated over the LAST one.
        for (const string& fileName : { util.CSVfileActivationPoint,
                                        util.CSVfileConnection,
                                        util.CSVfileNeuron,
                                        util.CSVfileSpikingNeuron }) {

            // FIX: join directory + filename with a separator
            fstream file(util.CSVfileDir + "\\" + fileName);
            if (file.fail()) {
                cout << "File " + fileName + " does not exist" << endl;
                indexer++;
                continue;
            }

            string text;
            // FIX: read EVERY line, not just the first
            while (getline(file, text)) {
                if (text.empty()) continue;

                vector<string> split_view = splitCSV(text);

                switch (indexer) {
                case 0: { // ActivationPoint — FIX: braces around each case
                    // FIX: stack object, not an uninitialized pointer.
                    // "ActivationPoint* ap;" pointed at garbage; writing to it
                    // and then delete-ing it is an instant crash.
                    ActivationPoint ap;
                    ap.ID        = stoi(split_view[0]);
                    ap.threshold = stof(split_view[1]);
                    ActivationPointInitalized.push_back(ap);
                    break;
                }
                case 1: { // Connection
                    Connection c;
                    c.ID     = stoi(split_view[0]);   // FIX: stoi/stof were missing
                    c.weight = stof(split_view[1]);
                    // columns 2+ would be your neuron ID lists, e.g. "1$2$3"
                    ConnectionsInitalized.push_back(c);
                    break;
                }
                case 2: { // Neuron
                    Neuron n;
                    n.ID        = stoi(split_view[0]);
                    n.threshold = stof(split_view[1]);
                    NeuronInitalized.push_back(n);
                    break;
                }
                case 3: { // SpikingNeuron
                    SpikingNeuron sn;
                    sn.ID = stoi(split_view[0]);
                    SpikingNeuronInitalized.push_back(sn);
                    break;
                }
                default:
                    cout << "Invalid file index" << endl;
                    break;
                }
            }
            file.close();
            indexer++;
        }

        // sanity check — always verify a loader actually loaded
        cout << "Loaded: " << ActivationPointInitalized.size()  << " activation points, "
             << ConnectionsInitalized.size()                    << " connections, "
             << NeuronInitalized.size()                         << " neurons, "
             << SpikingNeuronInitalized.size()                  << " spiking neurons" << endl;
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