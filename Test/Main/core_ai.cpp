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
        int inputItemIDs{};
        int outputItemIDs{};
    };

	struct Output {
        vector<int> outputItemIDs{};
	};

    struct Input {
        vector<int> inputItemIDs{};
		float threshold{};
		int tickPosition{}; // position in activation cycle, 0 is inactive, 1 is innitatied, beyond that follows the activation function
    };

    struct SpikingNeuron : Item, Output {
		bool active{}; // is active or not
    };

    struct Neuron : Item, Input, Output {
        float weight{};
        bool active{};
    };

    struct ActivationPoint : Item, Input {
		vector<int> outputItemIDs;
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
        if (input.inputItemIDs.empty()) {
            return {};
        }
        vector<int> inputConnections;
        for (int i : input.inputItemIDs) {
            inputConnections.push_back(i);
        }
        return inputConnections;
    }

    // Connection funcs
    int getConnectionID(Neuron input, Neuron target) {
        int ID;
        for (int connectID : getInputConnectionsID(input)) {
            Connection connect = IDtoConnection(connectID);
            if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
                ID = connect.ID;
            }
        }
        return ID;
    }

    int getConnectionID(SpikingNeuron input, Neuron target) {
        int ID;
        for (int connectID : getInputConnectionsID(input)) {
            Connection connect = IDtoConnection(connectID);
            if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
                ID = connect.ID;
            }
        }
        return ID;
    }

    int getConnectionID(Neuron input, ActivationPoint target) {
        int ID;
        for (int connectID : getInputConnectionsID(input)) {
            Connection connect = IDtoConnection(connectID);
            if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
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
                    ap.ID            = stoi(split_view[0]);
                    ap.threshold     = stof(split_view[1]);
                    ap.outputItemIDs = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.ActivationPointInitalized.push_back(ap);
                    break;
                }
                case 1: { // Connection
                    Connection c;
                    c.ID            = stoi(split_view[0]);
                    c.weight        = stof(split_view[1]);
                    c.inputItemIDs  = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    c.outputItemIDs = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.ConnectionsInitalized.push_back(c);
                    break;
                }
                case 2: { // Neuron
                    Neuron n;
                    n.ID            = stoi(split_view[0]);
                    n.threshold     = stof(split_view[1]);
                    n.inputItemIDs  = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    n.outputItemIDs = stoi(Ultimate.splitSpecial(split_view[2], '$'));
                    globalWeb.NeuronInitalized.push_back(n);
                    break;
                }
                case 3: { // SpikingNeuron
                    SpikingNeuron sn;
                    sn.ID           = stoi(split_view[0]);
                    sn.inputItemIDs = stoi(Ultimate.splitSpecial(split_view[2], '$'));
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

    void deleteConnection(Input& inputConnect, int connectionID) {
        vector<int> k = inputConnect.inputItemIDs;
        bool h = false;
        for (int i : inputConnect.inputItemIDs) {
            if (!connectionID == i) {
                k.push_back(i);
            } else {
                h = true;
            }
        }
        if (h) {
            cout << "No connection with " + std::to_string(connectionID) + " existed.";
        }
        while(inputConnect.inputItemIDs.empty() == 0) {
            inputConnect.inputItemIDs.pop_back();
        }
        for (int l : k) {
            inputConnect.inputItemIDs.push_back(l);
        }
    }

    void deleteConnection(Output& outputConnect, int connectionID) {
        vector<int> k = outputConnect.outputItemIDs;
        bool h = false;
        for (int i : outputConnect.outputItemIDs) {
            if (!connectionID == i) {
                k.push_back(i);
            } else {
                h = true;
            }
        }
        if (h) {
            cout << "No connection with " + std::to_string(connectionID) + " existed.";
        }
        while(outputConnect.outputItemIDs.empty() == 0) {
            outputConnect.outputItemIDs.pop_back();
        }
        for (int l : k) {
            outputConnect.outputItemIDs.push_back(l);
        }
    }

    void newInputConnection(Web& web, Input& inputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = inputConnect.inputItemIDs;
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

    void newOutputConnection(Web& web, Output& outputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = outputConnect.outputItemIDs;
        vector<int> k;

        switch(connectTo) {
            case 's' : {
                for (SpikingNeuron sn : web.SpikingNeuronInitalized) {
                    k.push_back(sn.ID);
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
        float p = n.weight * activationFunc(n.tickPosition);

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