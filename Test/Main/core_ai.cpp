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

    // constants / global needs
        int nextGlobalID = 1;
        const int maxNeuronlenght = 10;
    //

    // struct/enum

        enum class NodeKind { Neuron, SpikingNeuron, ActivationPoint, Connection};

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
            NodeKind inputKind{};
            NodeKind outputKind{};
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

        // Claude solution ??
        template<typename T>
        struct Store {
            std::vector<T> items;
            std::unordered_map<int, size_t> indexOf;

            T* find(int id) {
                auto it = indexOf.find(id);
                return (it != indexOf.end()) ? &items[it->second] : nullptr;
            }

            void add(T item) {
                indexOf[item.id] = items.size();
                items.push_back(std::move(item));
            }

            void remove(int id) {
                auto it = indexOf.find(id);
                if (it == indexOf.end()) return;
                items.erase(items.begin() + it->second);
                rebuildIndex();  // O(n), but removal only happens every ~25 ticks via webGrader
            }

            void rebuildIndex() {
                indexOf.clear();
                for (size_t i = 0; i < items.size(); i++) indexOf[items[i].id] = i;
            }

            // passthroughs so existing range-for / size / clear / empty call sites don't need to change
            size_t size() const { return items.size(); }
            bool empty() const { return items.empty(); }
            void clear() { items.clear(); indexOf.clear(); }
            auto begin() { return items.begin(); }
            auto end() { return items.end(); }
            auto begin() const { return items.begin(); }
            auto end() const { return items.end(); }
            T& operator[](size_t i) { return items[i]; }
            const T& operator[](size_t i) const { return items[i]; }
        };

        struct Web {
            Store<ActivationPoint> ActivationPointInitalized;
            Store<Connection> ConnectionsInitalized;
            Store<Neuron> NeuronInitalized;
            Store<SpikingNeuron> SpikingNeuronInitalized;
            int MaxConnections{};
        } globalWeb;

        struct SignalAccumulator {
            float total = 0.0f;
            vector<int> contributingConnections;
        };

        struct ItemRef {
            NodeKind kind;
            void* ptr;
        };
    //

    // ID to object mapping
        ItemRef IDtoItem(int ID) {
            if (Neuron* n = findNeuron(ID)) return { NodeKind::Neuron, n };
            if (SpikingNeuron* sn = findSpikingNeuron(ID)) return { NodeKind::SpikingNeuron, sn };
            if (ActivationPoint* ap = findActivationPoint(ID)) return { NodeKind::ActivationPoint, ap };
            if (Connection* c = findConnection(ID)) return { NodeKind::Connection, c };
            return { NodeKind::Neuron, nullptr }; // caller must check ptr
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

    // find
        Neuron*          findNeuron(int ID)          { return globalWeb.NeuronInitalized.find(ID); }
        Connection*      findConnection(int ID)      { return globalWeb.ConnectionsInitalized.find(ID); }
        SpikingNeuron*   findSpikingNeuron(int ID)   { return globalWeb.SpikingNeuronInitalized.find(ID); }
        ActivationPoint* findActivationPoint(int ID) { return globalWeb.ActivationPointInitalized.find(ID); }
    //

    // Connection funcs
        int getConnectionID(Neuron input, Neuron target) {
            for (int connectID : getOutputConnectionsID(input)) {
                Connection* connect = findConnection(connectID);
                if (connect && input.ID == connect->inputItemIDs && target.ID == connect->outputItemIDs) {
                    return connect->ID;
                }
            }
            return -1; // or std::optional<int>, matching your established pattern
        }

        // int getConnectionID(SpikingNeuron input, Neuron target) {
        //     int ID;
        //     for (int connectID : getOutputConnectionsID(input)) {
        //         Connection* connect = findConnection(connectID);
        //         if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
        //             ID = connect.ID;
        //         }
        //     }
        //     return ID;
        // }

        // int getConnectionID(Neuron input, ActivationPoint target) {
        //     int ID;
        //     for (int connectID : getOutputConnectionsID(input)) {
        //         Connection connect = findConnection(connectID);
        //         if ((input.ID == connect.inputItemIDs) && (target.ID == connect.outputItemIDs)) { 
        //             ID = connect.ID;
        //         }
        //     }
        //     return ID;
        // }
    //

    int allocateID() {
        return nextGlobalID++;
    }

    void syncIDCounter(int usedID) {
        if (usedID >= nextGlobalID) nextGlobalID = usedID + 1;
    }

    void generateDefaultNetwork() {
        SpikingNeuron sn; sn.ID = allocateID(); sn.period = 4;
        Neuron n;         n.ID    = allocateID(); n.threshold = 0.5f; n.weight = 1.0f;
        ActivationPoint ap; ap.ID = allocateID(); ap.threshold = 0.5f;

        globalWeb.SpikingNeuronInitalized.add(sn);
        globalWeb.NeuronInitalized.add(n);
        globalWeb.ActivationPointInitalized.add(ap);

        connectNodes(sn.ID, NodeKind::SpikingNeuron, n.ID, NodeKind::Neuron, 1.0f);
        connectNodes(n.ID,  NodeKind::Neuron, ap.ID, NodeKind::ActivationPoint, 1.0f);

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
                    syncIDCounter(ap.ID);
                    globalWeb.ActivationPointInitalized.add(ap);
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
                    syncIDCounter(c.ID);
                    globalWeb.ConnectionsInitalized.add(c);
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
                    syncIDCounter(n.ID);
                    globalWeb.NeuronInitalized.add(n);
                    break;
                }
                case 3: { // SpikingNeuron
                    SpikingNeuron sn;
                    sn.ID = stoi(split_view[0]);
                    for (string i : util.splitSpecial(split_view[2], '$')) {
                        sn.outputItemIDs.push_back(stoi(i));
                    }
                    syncIDCounter(sn.ID);
                    globalWeb.SpikingNeuronInitalized.add(sn);
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

        globalWeb.ConnectionsInitalized.remove(connectionID);  // was the remove_if/erase block
    }

    void newInputConnection(Input& inputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = inputConnect.inputItemIDs;
        vector<int> k;

        switch(connectTo) {
            case 'a' : {
                for (ActivationPoint ap : globalWeb.ActivationPointInitalized) {
                    k.push_back(ap.ID);
                }
                break;
            }
            case 'n' : {
                for (Neuron n : globalWeb.NeuronInitalized) {
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

    void newOutputConnection(Output& outputConnect, char connectTo, int deleteConnection) {
        vector<int> currentConnections = outputConnect.outputItemIDs;
        vector<int> k;

        switch(connectTo) {
            case 's' : {
                for (SpikingNeuron sn : globalWeb.SpikingNeuronInitalized) {
                    k.push_back(sn.ID);
                break;
                }
            }
            case 'n' : {
                for (Neuron n : globalWeb.NeuronInitalized) {
                    k.push_back(n.ID);
                }
                break;
            }
            default : {
                cout << "connectTo is invalid.";
            }
        }
    }

    void initNewNetwork(int activationPointCount, int connectionCount, int neuronCount, int spikingNeuronCount, int MaxConnection, char typeOfMaxConnections) {
        switch (typeOfMaxConnections) {
            case 'i': {
                // int
                globalWeb.MaxConnections = MaxConnection;
                break;
            }
            case '%': {
                // percentage
                globalWeb.MaxConnections = (int)(MaxConnection / neuronCount);
                break;
            }
        }
    }
    
    vector<float> buildActivationLUT() {
        vector<float> helper;
        for (int i = 0; i <= maxNeuronlenght; i++) {
            float tickOffset = 1.0f;
            float r = 1.04f;
            float mult = 2.0f;
            float hump = pow(2, (-1 * (i - tickOffset)) / r * 4.41f) - pow(2, (-1 * (i - tickOffset)) / r);
            if (hump < 0) hump = 0;
            else if (hump > 1) hump = 1;
            hump *= mult;
            helper.push_back(hump);
        }
        return helper;
    }

    vector<float> activationLUT = buildActivationLUT();
    float activationFunc(int i) {return activationLUT[i];}

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

    int addRandomNeuron(Web& globalWeb) {
        Neuron n;
        n.ID = allocateID();
        n.threshold = util.randomFloat(0.2f, 0.8f);
        n.weight    = util.randomFloat(0.5f, 1.5f);
        globalWeb.NeuronInitalized.add(n);
        cout << "Added random Neuron " << n.ID << " (threshold=" << n.threshold
            << ", weight=" << n.weight << ")" << endl;
        return n.ID;
    }

    int addRandomSpikingNeuron(Web& globalWeb) {
        SpikingNeuron sn;
        sn.ID = allocateID();
        sn.period = 2 + (int)(util.rng() % 6); // fires somewhere every 2-7 ticks
        globalWeb.SpikingNeuronInitalized.add(sn);
        cout << "Added random SpikingNeuron " << sn.ID << " (period=" << sn.period << ")" << endl;
        return sn.ID;
    }

    int addActivationPoint(float threshold) {
        ActivationPoint ap;
        ap.ID = allocateID();
        ap.threshold = threshold;
        globalWeb.ActivationPointInitalized.add(ap);
        cout << "Added ActivationPoint " << ap.ID << " (threshold=" << threshold << ")" << endl;
        return ap.ID;
    }

    void resetWeb(Web& globalWeb) {
        globalWeb.ActivationPointInitalized.clear();
        globalWeb.ConnectionsInitalized.clear();
        globalWeb.NeuronInitalized.clear();
        globalWeb.SpikingNeuronInitalized.clear();
        globalWeb.MaxConnections = 0;
        nextGlobalID = 1;
        cout << "Web cleared." << endl;
    }

    bool connectNodes(int sourceID, NodeKind sourceKind, int targetID, NodeKind targetKind,
                      float weight = -1.0f) {
        Connection c;
        c.ID = allocateID();
        c.weight = (weight >= 0.0f) ? weight : util.randomFloat(0.4f, 1.0f);
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

        globalWeb.ConnectionsInitalized.add(c);
        return true;
    }

    bool webGrader(Web& globalWeb) {
        static int ticksSinceLastGrowth = 0;
        const int GROWTH_COOLDOWN_TICKS = 25;

        ticksSinceLastGrowth++;
        if (ticksSinceLastGrowth < GROWTH_COOLDOWN_TICKS) {
            return false; // not time to prune/grow yet
        }
        ticksSinceLastGrowth = 0;

        vector<int> weakConnections;
        for (Connection& c : globalWeb.ConnectionsInitalized) {
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
                "exportTo has " + to_string(exportTo->size()) + " slots but the globalWeb has " +
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

        globalWeb.SpikingNeuronInitalized.clear();
        globalWeb.SpikingNeuronInitalized.add(snA);
        globalWeb.SpikingNeuronInitalized.add(snB);

        globalWeb.NeuronInitalized.clear();
        globalWeb.NeuronInitalized.add(nA);
        globalWeb.NeuronInitalized.add(nB);

        globalWeb.ActivationPointInitalized.clear();
        globalWeb.ActivationPointInitalized.add(apA);
        globalWeb.ActivationPointInitalized.add(apB);

        // fixed weight=1.0 throughout, deliberately -- this is a verification test,
        // not a growth experiment, so nothing here should be left to random chance
        connectNodes(snA.ID, NodeKind::SpikingNeuron, nA.ID,  NodeKind::Neuron, 1.0f);
        connectNodes(snB.ID, NodeKind::SpikingNeuron, nB.ID,  NodeKind::Neuron, 1.0f);
        connectNodes(nA.ID,  NodeKind::Neuron,        apA.ID, NodeKind::ActivationPoint, 1.0f);
        connectNodes(nB.ID,  NodeKind::Neuron,        apB.ID, NodeKind::ActivationPoint, 1.0f);

        bool runAI = true;
        vector<float> exportBuffer(globalWeb.ActivationPointInitalized.size(), 0.0f);

        for (int t = 0; t < 20 && runAI; t++) {
            update(t, &exportBuffer);
            cout << "tick " << t << ": A=" << exportBuffer[0] << " B=" << exportBuffer[1] << endl;
        }
    }
};