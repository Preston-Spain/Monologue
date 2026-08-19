using namespace std;
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Util.h"

class StateStreamTransformer {
public:

	struct Item {
		int ID{}; // Identifier
	};
    
    struct Connection : Item {
        float weight{};
        int inputItemID{};
        int outputItemID{};
    };

	struct Output {
        vector<int> outputItemID{};
		float threshold{};
	};

    struct Input {
        vector<int> inputItemID{};
		int tickPosition{}; // position in activation cycle, 0 is inactive, 1 is innitatied, beyond that follows the activation function
    };

    struct SpikingNeuron : Item, Input {
		bool active{}; // is active or not
    };

    struct Neuron : Item, Input, Output {
        bool active{};
    };

    struct ActivationPoint : Item, Output {
		vector<int> outputItemID;
        int tickPosition{};
        bool active{};
    };

    struct Web {
        vector<ActivationPoint> ActivationPointInitalized;
        vector<Connection> ConnectionsInitalized;
        vector<Neuron> NeuronInitalized;
        vector<SpikingNeuron> SpikingNeuronInitalized;
        int MaxConnections{};
    } globalWeb;

    ActivationPoint IDtoActivationPoint(int ID) {
        for (ActivationPoint c : globalWeb.ActivationPointInitalized) {
            if (c.ID == ID) {
                return c;
            }
        }
        return {};
    }

    Connection IDtoConnection(int ID) {
        for (Connection c : globalWeb.ConnectionsInitalized) {
            if (c.ID == ID) {
                return c;
            }
        }
        return {};
    }

    Neuron IDtoNeuron(int ID) {
        for (Neuron c : globalWeb.NeuronInitalized) {
            if (c.ID == ID) {
                return c;
            }
        }
        return {};
    }

    SpikingNeuron IDtoSpikingNeuron(int ID) {
        for (SpikingNeuron c : globalWeb.SpikingNeuronInitalized) {
            if (c.ID == ID) {
                return c;
            }
        }
        return {};
    }

    vector<int> getInputConnectionsID(Input input) {
        if (input.inputItemID.empty()) {
            return {};
        }
        vector<int> inputConnections;
        for (int i : input.inputItemID) {
            inputConnections.push_back(i);
        }
        return inputConnections;
    }

    int getConnectionID(Input input, Output target) {
        int ID;
        for (int connectID : getInputConnectionsID(input)) {
            Connection connect = IDtoConnection(connectID);
            if ((input == connect.inputItemID) && (target == connect.outputItemID)) { // ! needs to be output to output not output to int, and specification on which type it is
                ID = connect.ID;
            }
        }
        return ID;
    }

    void init() {
        int indexer = 0;

        for (const string& fileName : { Ultimate.CSVfileActivationPoint,
                                        Ultimate.CSVfileConnection,
                                        Ultimate.CSVfileNeuron,
                                        Ultimate.CSVfileSpikingNeuron }) {

            fstream file(Ultimate.CSVfileDir + "\\" + fileName);
            if (file.fail()) {
                cout << "File " + fileName + " does not exist" << endl;
                indexer++;
                continue;
            }

            string text;
            while (getline(file, text)) {
                if (text.empty()) continue;

                vector<string> split_view = Ultimate.splitCSV(text);

                switch (indexer) {
                case 0: { // ActivationPoint
                    ActivationPoint ap;
                    ap.ID           = stoi(split_view[0]);
                    ap.threshold    = stof(split_view[1]);
                    ap.outputItemID = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.ActivationPointInitalized.push_back(ap);
                    break;
                }
                case 1: { // Connection
                    Connection c;
                    c.ID           = stoi(split_view[0]);
                    c.weight       = stof(split_view[1]);
                    c.inputItemID  = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    c.outputItemID = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.ConnectionsInitalized.push_back(c);
                    break;
                }
                case 2: { // Neuron
                    Neuron n;
                    n.ID           = stoi(split_view[0]);
                    n.threshold    = stof(split_view[1]);
                    n.inputItemID  = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    n.outputItemID = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.NeuronInitalized.push_back(n);
                    break;
                }
                case 3: { // SpikingNeuron
                    SpikingNeuron sn;
                    sn.ID          = stoi(split_view[0]);
                    sn.inputItemID = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.SpikingNeuronInitalized.push_back(sn);
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
        cout << "Loaded: "                                 << endl
             << globalWeb.ActivationPointInitalized.size() << " activation points, " << endl
             << globalWeb.ConnectionsInitalized.size()     << " connections, "       << endl
             << globalWeb.NeuronInitalized.size()          << " neurons, "           << endl
             << globalWeb.SpikingNeuronInitalized.size()   << " spiking neurons"     << endl;
    }

	void update() {
		// update the state of the neurons and activation points based on the current tick
		return;
	}

    void deleteInputConnection(Input& inputConnect) {
        for (:) {
            //
        }
    }

    void newInputConnection(Web& web, Input& inputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = inputConnect.inputItemID;
        vector<int> k;

        switch(connectTo) {
            case 'a' : {
                for (ActivationPoint ap : web.ActivationPointInitalized) {
                    k.push_back(ap.ID);
                }
            }
            case 'n' : {
                for (Neuron n : web.NeuronInitalized) {
                    k.push_back(n.ID);
                }
            }
            default : {
                cout << "connectTo is invalid.";
            }
        }

        for (int i : currentConnections) {
            //
        }
    }

    void initNewNetwork(Web& web, int activationPointCount, int connectionCount, int neuronCount, int spikingNeuronCount, int MaxConnection, char typeOfMaxConnections) {
        switch (typeOfMaxConnections) {
            case 'i': {
                // int
                web.MaxConnections = MaxConnection;
            }
            case '%': {
                // percentage
                web.MaxConnections = (int)(MaxConnection / neuronCount);
            }
        }
    }

    float activationFunc(int tickPosition) {
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

    float neuron(float input, Neuron& n, int currentTick) {
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

    bool webT(Web web) {
        //
    }

    bool webGrader(Web web) {}

    int run() {
        //
        return 0;
    }
};

int main() {
  return 0;
}