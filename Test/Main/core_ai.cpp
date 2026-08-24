using namespace std;
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include "Util.h"


class StateStreamTransformer {
public:
    // sister File imports
        Util util;
    //

    // struct
        struct Item {
            int ID{}; // Identifier
        };
        
        struct Connection : Item {
            float weight{};
            int inputItemIDs{};
            int outputItemIDs{};
            int firedCount{};
            int succeededCount{};
            float activityScore{};
            bool pacemakerSourced{};
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
            int firedCount{};
            int period{};
            int ticksSinceLastFire{};
        };

        struct Neuron : Item, Input, Output {
            float weight{};
            bool active{};
            int firedCount{};
            int subthresholdCount{};
        };

        struct ActivationPoint : Item, Input {
            vector<int> outputItemIDs;
            bool active{};
            int inputCount{};
            float outputStrength{};
        };

        enum class NodeKind { Neuron, SpikingNeuron, ActivationPoint };

        struct Web {
            vector<ActivationPoint> ActivationPointInitalized;
            vector<Connection> ConnectionsInitalized;
            vector<Neuron> NeuronInitalized;
            vector<SpikingNeuron> SpikingNeuronInitalized;
            int MaxConnections{};
        } globalWeb;

        struct SignalAccumulator {
            float total = 0.0f;
            vector<int> contributingConnections;
        };
    //

    // ID to object mapping
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
    //

    // get connections ID
        vector<int> getOutputConnectionsID(Output output) {
            if (output.outputItemIDs.empty()) {
                return {};
            }
            vector<int> outputConnections;
            for (int i : output.outputItemIDs) {
                outputConnections.push_back(i);
            }
            return outputConnections;
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
    //

    bool spikingNeuronFires(SpikingNeuron& sn) {
        sn.ticksSinceLastFire++;
        if (sn.ticksSinceLastFire >= sn.period) {
            sn.ticksSinceLastFire = 0;
            return true;
        }
        return false;
    }

    void activationPoint(float input, ActivationPoint& ap) {
        ap.active = (input >= ap.threshold);
        ap.outputStrength = input;
        ap.inputCount++;
    }

    float neuronEmission(const Neuron& n) {
        return (n.tickPosition > 0) ? n.weight * activationFunc(n.tickPosition) : 0.0f;
    }

    int activationPointCount() const {
        return (int)globalWeb.ActivationPointInitalized.size();
    }

    // class*
        Neuron* findNeuron(int ID) {
            for (Neuron& n : globalWeb.NeuronInitalized) {
                if (n.ID == ID) return &n;
            }
            return nullptr;
        }

        Connection* findConnection(int ID) {
            for (Connection& c : globalWeb.ConnectionsInitalized) {
                if (c.ID == ID) return &c;
            }
            return nullptr;
        }

        SpikingNeuron* findSpikingNeuron(int ID) {
            for (SpikingNeuron& sn : globalWeb.SpikingNeuronInitalized) {
                if (sn.ID == ID) return &sn;
            }
            return nullptr;
        }

        ActivationPoint* findActivationPoint(int ID) {
            for (ActivationPoint& ap : globalWeb.ActivationPointInitalized) {
                if (ap.ID == ID) return &ap;
            }
            return nullptr;
        }
    //

    // Connection funcs
        int getConnectionID(Neuron input, Neuron target) {
            int ID;
            for (int connectID : getOutputConnectionsID(input)) {
                Connection connect = IDtoConnection(connectID);
                if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
                    ID = connect.ID;
                }
            }
            return ID;
        }

        int getConnectionID(SpikingNeuron input, Neuron target) {
            int ID;
            for (int connectID : getOutputConnectionsID(input)) {
                Connection connect = IDtoConnection(connectID);
                if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
                    ID = connect.ID;
                }
            }
            return ID;
        }

        int getConnectionID(Neuron input, ActivationPoint target) {
            int ID;
            for (int connectID : getOutputConnectionsID(input)) {
                Connection connect = IDtoConnection(connectID);
                if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
                    ID = connect.ID;
                }
            }
            return ID;
        }
    //

    void generateDefaultNetwork() {
        SpikingNeuron sn; sn.ID = allocateID(); sn.period = 4;
        Neuron n;         n.ID    = allocateID(); n.threshold = 0.5f; n.weight = 1.0f;
        ActivationPoint ap; ap.ID = allocateID(); ap.threshold = 0.5f;

        globalWeb.SpikingNeuronInitalized.push_back(sn);
        globalWeb.NeuronInitalized.push_back(n);
        globalWeb.ActivationPointInitalized.push_back(ap);

        connectNodes(globalWeb, sn.ID, NodeKind::SpikingNeuron, n.ID, NodeKind::Neuron, 1.0f);
        connectNodes(globalWeb, n.ID,  NodeKind::Neuron, ap.ID, NodeKind::ActivationPoint, 1.0f);

        cout << "Generated default network: SpikingNeuron " << sn.ID
            << " -> Neuron " << n.ID << " -> ActivationPoint " << ap.ID << endl;
    }

    void init() {
        int indexer = 0;

        for (const string& fileName : { util.CSVfileActivationPoint,
                                        util.CSVfileConnection,
                                        util.CSVfileNeuron,
                                        util.CSVfileSpikingNeuron }) {

            fstream file(util.CSVfileDir + "\\" + fileName);
            if (file.fail()) {
                cout << "File " + fileName + " does not exist" << endl;
                indexer++;
                continue;
            }

            string text;
            while (getline(file, text)) {
                if (text.empty()) continue;

                vector<string> split_view = util.splitCSV(text);

                switch (indexer) {
                case 0: { // ActivationPoint
                    ActivationPoint ap;
                    ap.ID            = stoi(split_view[0]);
                    ap.threshold     = stof(split_view[1]);
                    for (string i : util.splitSpecial(split_view[2], '$')) {
                        ap.outputItemIDs.push_back(stoi(i));
                    }
                    globalWeb.ActivationPointInitalized.push_back(ap);
                    break;
                }
                case 1: { // Connection
                    Connection c;
                    c.ID             = stoi(split_view[0]);
                    c.weight         = stof(split_view[1]);
                    c.inputItemIDs   = stoi(split_view[2]);
                    c.outputItemIDs  = stoi(split_view[3]);
                    c.firedCount     = 0;
                    c.succeededCount = 0;
                    globalWeb.ConnectionsInitalized.push_back(c);
                    break;
                }
                case 2: { // Neuron
                    Neuron n;
                    n.ID            = stoi(split_view[0]);
                    n.threshold     = stof(split_view[1]);
                    for (string i : util.splitSpecial(split_view[2], '$')) {
                        n.inputItemIDs.push_back(stoi(i));
                    }
                    for (string i : util.splitSpecial(split_view[3], '$')) {
                        n.outputItemIDs.push_back(stoi(i));
                    }
                    n.firedCount = 0;
                    n.subthresholdCount = 0;
                    globalWeb.NeuronInitalized.push_back(n);
                    break;
                }
                case 3: { // SpikingNeuron
                    SpikingNeuron sn;
                    sn.ID = stoi(split_view[0]);
                    for (string i : util.splitSpecial(split_view[2], '$')) {
                        sn.outputItemIDs.push_back(stoi(i));
                    }
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

    void load() {
        init();
        if (globalWeb.NeuronInitalized.empty() &&
            globalWeb.SpikingNeuronInitalized.empty() &&
            globalWeb.ActivationPointInitalized.empty()) {
            cout << "No network found on disk -- generating a default network." << endl;
            generateDefaultNetwork();
        }
    }

    void deleteConnection(Input& inputConnect, int connectionID) {
        size_t before = inputConnect.inputItemIDs.size();
        vector<int> kept;
        for (int i : inputConnect.inputItemIDs) {
            if (i != connectionID) kept.push_back(i);
        }
        inputConnect.inputItemIDs = kept;
        if (inputConnect.inputItemIDs.size() == before) {
            cout << "No connection with " << connectionID << " existed." << endl;
        }
    }

    void deleteConnection(Output& outputConnect, int connectionID) {
        size_t before = outputConnect.outputItemIDs.size();
        vector<int> kept;
        for (int i : outputConnect.outputItemIDs) {
            if (i != connectionID) kept.push_back(i);
        }
        outputConnect.outputItemIDs = kept;
        if (outputConnect.outputItemIDs.size() == before) {
            cout << "No connection with " << connectionID << " existed." << endl;
        }
    }

    void removeConnection(int connectionID) {
        Connection* c = findConnection(connectionID);
        if (!c) {
            cout << "No connection with " << connectionID << " existed." << endl;
            return;
        }

        if (Neuron* src = findNeuron(c->inputItemIDs)) {
            deleteConnection(static_cast<Output&>(*src), connectionID);
        } else if (SpikingNeuron* src = findSpikingNeuron(c->inputItemIDs)) {
            deleteConnection(static_cast<Output&>(*src), connectionID);
        }

        if (Neuron* tgt = findNeuron(c->outputItemIDs)) {
            deleteConnection(static_cast<Input&>(*tgt), connectionID);
        } else if (ActivationPoint* tgt = findActivationPoint(c->outputItemIDs)) {
            deleteConnection(static_cast<Input&>(*tgt), connectionID);
        }

        auto& conns = globalWeb.ConnectionsInitalized;
        conns.erase(std::remove_if(conns.begin(), conns.end(),
            [connectionID](const Connection& x) { return x.ID == connectionID; }),
            conns.end());
    }

    void newInputConnection(Web& web, Input& inputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = inputConnect.inputItemIDs;
        vector<int> k;

        switch(connectTo) {
            case 'a' : {
                for (ActivationPoint ap : web.ActivationPointInitalized) {
                    k.push_back(ap.ID);
                }
                break;
            }
            case 'n' : {
                for (Neuron n : web.NeuronInitalized) {
                    k.push_back(n.ID);
                }
                break;
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
                break;
                }
            }
            case 'n' : {
                for (Neuron n : web.NeuronInitalized) {
                    k.push_back(n.ID);
                }
                break;
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
                break;
            }
            case '%': {
                // percentage
                web.MaxConnections = (int)(MaxConnection / neuronCount);
                break;
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

        n.subthresholdCount++;

        if (n.tickPosition == 0) { // init
            if (input >= n.threshold) {
                n.tickPosition = 1;
                n.subthresholdCount--;
                n.firedCount++;
                return p;
            }
        } else if (n.tickPosition > 0) {
            if (input >= n.threshold) {
                n.tickPosition = 1;
                n.subthresholdCount--;
                n.firedCount++;
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
    
    void updateConnectionGrade(Connection& c, bool succeeded) {
        const float decay = 0.95f; // higher = remembers longer, tune by feel

        c.firedCount++;
        if (succeeded) c.succeededCount++;

        float instant = succeeded ? 1.0f : 0.0f;
        c.activityScore = decay * c.activityScore + (1.0f - decay) * instant;
    }

    int addRandomNeuron(Web& web) {
        Neuron n;
        n.ID = allocateID();
        n.threshold = randomFloat(0.2f, 0.8f);
        n.weight    = randomFloat(0.5f, 1.5f);
        web.NeuronInitalized.push_back(n);
        cout << "Added random Neuron " << n.ID << " (threshold=" << n.threshold
            << ", weight=" << n.weight << ")" << endl;
        return n.ID;
    }

    int addRandomSpikingNeuron(Web& web) {
        SpikingNeuron sn;
        sn.ID = allocateID();
        sn.period = 2 + (int)(rng() % 6); // fires somewhere every 2-7 ticks
        web.SpikingNeuronInitalized.push_back(sn);
        cout << "Added random SpikingNeuron " << sn.ID << " (period=" << sn.period << ")" << endl;
        return sn.ID;
    }

    int addActivationPoint(Web& web, float threshold) {
        ActivationPoint ap;
        ap.ID = allocateID();
        ap.threshold = threshold;
        web.ActivationPointInitalized.push_back(ap);
        cout << "Added ActivationPoint " << ap.ID << " (threshold=" << threshold << ")" << endl;
        return ap.ID;
    }

    void resetWeb(Web& web) {
        web.ActivationPointInitalized.clear();
        web.ConnectionsInitalized.clear();
        web.NeuronInitalized.clear();
        web.SpikingNeuronInitalized.clear();
        web.MaxConnections = 0;
        nextGlobalID = 1;
        cout << "Web cleared." << endl;
    }

    bool connectNodes(Web& web, int sourceID, NodeKind sourceKind, int targetID, NodeKind targetKind,
                      float weight = -1.0f) {
        Connection c;
        c.ID = allocateID();
        c.weight = (weight >= 0.0f) ? weight : randomFloat(0.4f, 1.0f);
        c.inputItemIDs = sourceID;
        c.outputItemIDs = targetID;
        c.pacemakerSourced = (sourceKind == NodeKind::SpikingNeuron);

        switch (sourceKind) {
            case NodeKind::SpikingNeuron: {
                SpikingNeuron* src = findSpikingNeuron(sourceID);
                if (!src) return false;
                src->outputItemIDs.push_back(c.ID);
                break;
            }
            case NodeKind::Neuron: {
                Neuron* src = findNeuron(sourceID);
                if (!src) return false;
                src->outputItemIDs.push_back(c.ID);
                break;
            }
            default:
                cout << "Invalid source kind for a connection." << endl;
                return false;
        }

        switch (targetKind) {
            case NodeKind::Neuron: {
                Neuron* tgt = findNeuron(targetID);
                if (!tgt) return false;
                tgt->inputItemIDs.push_back(c.ID);
                break;
            }
            case NodeKind::ActivationPoint: {
                ActivationPoint* tgt = findActivationPoint(targetID);
                if (!tgt) return false;
                tgt->inputItemIDs.push_back(c.ID);
                break;
            }
            default:
                cout << "Invalid target kind for a connection." << endl;
                return false;
        }

        web.ConnectionsInitalized.push_back(c);
        return true;
    }

    bool webGrader(Web& web) {
        static int ticksSinceLastGrowth = 0;
        const int GROWTH_COOLDOWN_TICKS = 25;

        ticksSinceLastGrowth++;
        if (ticksSinceLastGrowth < GROWTH_COOLDOWN_TICKS) {
            return false; // not time to prune/grow yet
        }
        ticksSinceLastGrowth = 0;

        vector<int> weakConnections;
        for (Connection& c : web.ConnectionsInitalized) {
            if (c.pacemakerSourced) continue;      // never evict on metrics alone
            if (c.firedCount == 0) continue;       // hasn't had a chance to prove itself yet

            if (c.activityScore < 0.1f) {
                weakConnections.push_back(c.ID);
            }
        }

        for (int id : weakConnections) {
            removeConnection(id);  // safe: collected first, removed after the loop
        }

        return !weakConnections.empty();
    }

    void propagateSignal(const vector<int>& outgoingConnectionIDs, float sourceEmission,
                        unordered_map<int, SignalAccumulator>& incoming) {
        if (sourceEmission == 0.0f) return;
        for (int connID : outgoingConnectionIDs) {
            Connection* c = findConnection(connID);
            if (!c) continue;
            c->firedCount++;
            incoming[c->outputItemIDs].total += sourceEmission * c->weight;
            incoming[c->outputItemIDs].contributingConnections.push_back(connID);
        }
    }

    void update(int currentTick, vector<float>* exportTo = nullptr) {
        unordered_map<int, SignalAccumulator> incoming;

        // Phase 1: pacemakers emit
        for (SpikingNeuron& sn : globalWeb.SpikingNeuronInitalized) {
            if (!spikingNeuronFires(sn)) continue;
            sn.firedCount++;
            propagateSignal(sn.outputItemIDs, 1.0f, incoming);
        }

        // Phase 2: neurons already mid-fire keep emitting based on where they are in the curve
        for (Neuron& n : globalWeb.NeuronInitalized) {
            propagateSignal(n.outputItemIDs, neuronEmission(n), incoming);
        }

        // Phase 3: resolve each neuron's new state from what it just received
        for (Neuron& n : globalWeb.NeuronInitalized) {
            auto it = incoming.find(n.ID);
            float input = (it != incoming.end()) ? it->second.total : 0.0f;

            bool wasResting = (n.tickPosition == 0);
            neuron(input, n, currentTick);
            bool justFired = wasResting && (n.tickPosition == 1);

            if (it != incoming.end()) {
                for (int connID : it->second.contributingConnections) {
                    if (Connection* c = findConnection(connID)) {
                        updateConnectionGrade(*c, justFired);
                    }
                }
            }
        }

        // Phase 4: resolve activation points the same way
        for (size_t i = 0; i < globalWeb.ActivationPointInitalized.size(); i++) {
            ActivationPoint& ap = globalWeb.ActivationPointInitalized[i];
            auto it = incoming.find(ap.ID);
            float input = (it != incoming.end()) ? it->second.total : 0.0f;
            activationPoint(input, ap);
            if (exportTo) (*exportTo)[i] = ap.outputStrength;
        }
    }

    int run(bool& runAI, vector<float>* exportTo = nullptr) {
        if (exportTo && exportTo->size() != globalWeb.ActivationPointInitalized.size()) {
            throw std::invalid_argument(
                "exportTo has " + to_string(exportTo->size()) + " slots but the web has " +
                to_string(globalWeb.ActivationPointInitalized.size()) + " ActivationPoints.");
        }

        int tick = 0;
        while (runAI) {
            update(tick, exportTo);
            tick++;
        }
        return 0;
    }

    void runOscillationTest() {
        resetWeb(globalWeb);

        SpikingNeuron snA; snA.ID = allocateID(); snA.period = 4;
        SpikingNeuron snB; snB.ID = allocateID(); snB.period = 4;
        snB.ticksSinceLastFire = 2; // starts half a period ahead of A, so they alternate

        Neuron nA; nA.ID = allocateID(); nA.threshold = 0.5f; nA.weight = 1.0f;
        Neuron nB; nB.ID = allocateID(); nB.threshold = 0.5f; nB.weight = 1.0f;

        ActivationPoint apA; apA.ID = allocateID(); apA.threshold = 0.5f;
        ActivationPoint apB; apB.ID = allocateID(); apB.threshold = 0.5f;

        globalWeb.SpikingNeuronInitalized   = { snA, snB };
        globalWeb.NeuronInitalized          = { nA, nB };
        globalWeb.ActivationPointInitalized = { apA, apB };

        // fixed weight=1.0 throughout, deliberately -- this is a verification test,
        // not a growth experiment, so nothing here should be left to random chance
        connectNodes(globalWeb, snA.ID, NodeKind::SpikingNeuron, nA.ID,  NodeKind::Neuron, 1.0f);
        connectNodes(globalWeb, snB.ID, NodeKind::SpikingNeuron, nB.ID,  NodeKind::Neuron, 1.0f);
        connectNodes(globalWeb, nA.ID,  NodeKind::Neuron,        apA.ID, NodeKind::ActivationPoint, 1.0f);
        connectNodes(globalWeb, nB.ID,  NodeKind::Neuron,        apB.ID, NodeKind::ActivationPoint, 1.0f);

        bool runAI = true;
        vector<float> exportBuffer(globalWeb.ActivationPointInitalized.size(), 0.0f);

        for (int t = 0; t < 20 && runAI; t++) {
            update(t, &exportBuffer);
            cout << "tick " << t << ": A=" << exportBuffer[0] << " B=" << exportBuffer[1] << endl;
        }
    }
};