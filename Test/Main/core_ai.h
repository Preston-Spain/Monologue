#ifndef CORE_AI_H
#define CORE_AI_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Util.h"

class StateStreamTransformer {
public:
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
            std::vector<int> outputItemIDs{};
        };

        struct Input {
            std::vector<int> inputItemIDs{};
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
            std::vector<int> outputItemIDs;
            bool active{};
            int inputCount{};
            float outputStrength{};
        };

        // Kept inline (not split into the .cpp): this is a class template, so its
        // member bodies need to be visible wherever Store<T> is used unless we
        // explicitly instantiate it for every T in the .cpp. Not worth the ceremony
        // for boilerplate this small -- revisit if Store grows real logic.
        template<typename T>
        struct Store {
            std::vector<T> items;
            std::unordered_map<int, size_t> indexOf;

            T* find(int id) {
                auto it = indexOf.find(id);
                return (it != indexOf.end()) ? &items[it->second] : nullptr;
            }

            void add(T item) {
                indexOf[item.ID] = items.size();
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
                for (size_t i = 0; i < items.size(); i++) indexOf[items[i].ID] = i;
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
        };

        struct SignalAccumulator {
            float total = 0.0f;
            std::vector<int> contributingConnections;
        };

        struct ItemRef {
            NodeKind kind;
            void* ptr;
        };
    //

    // sister File imports + constants / global needs
        // Declaration order here matters: activationLUT's default initializer calls
        // buildActivationLUT(), which reads maxNeuronlenght -- so maxNeuronlenght
        // must be declared (and therefore initialized) before activationLUT.
        Util util;
        int nextGlobalID = 1;
        const int maxNeuronlenght = 10;
        Web globalWeb;
        std::vector<float> activationLUT = buildActivationLUT();
    //

    // ID to object mapping
        ItemRef IDtoItem(int ID);
    //

    // get connections ID
        std::vector<int> getOutputConnectionsID(Output output);
        std::vector<int> getInputConnectionsID(Input input);
    //

    bool spikingNeuronFires(SpikingNeuron& sn);
    void activationPoint(float input, ActivationPoint& ap);
    float neuronEmission(const Neuron& n);
    int activationPointCount() const;

    // find
        Neuron*          findNeuron(int ID);
        Connection*      findConnection(int ID);
        SpikingNeuron*   findSpikingNeuron(int ID);
        ActivationPoint* findActivationPoint(int ID);
    //

    // Connection funcs
        int getConnectionID(Neuron input, Neuron target);
    //

    int allocateID();
    void syncIDCounter(int usedID);
    void generateDefaultNetwork();
    void init();
    void load();

    void deleteConnection(Input& inputConnect, int connectionID);
    void deleteConnection(Output& outputConnect, int connectionID);
    void removeConnection(int connectionID);

    void newInputConnection(Input& inputConnect, char connectTo, int deleteConnection);
    void newOutputConnection(Output& outputConnect, char connectTo, int deleteConnection);

    void initNewNetwork(int activationPointCount, int connectionCount, int neuronCount,
                         int spikingNeuronCount, int MaxConnection, char typeOfMaxConnections);

    std::vector<float> buildActivationLUT();
    float activationFunc(int i);

    float neuron(float input, Neuron& n, int currentTick);

    void updateConnectionGrade(Connection& c, bool succeeded);

    int addRandomNeuron();
    int addRandomSpikingNeuron();
    int addActivationPoint(float threshold);

    void resetWeb();

    bool connectNodes(int sourceID, NodeKind sourceKind, int targetID, NodeKind targetKind,
                       float weight = -1.0f);

    bool webGrader();

    void propagateSignal(const std::vector<int>& outgoingConnectionIDs, float sourceEmission,
                          std::unordered_map<int, SignalAccumulator>& incoming);

    void update(int currentTick, std::vector<float>* exportTo = nullptr);

    int run(bool& runAI, std::vector<float>* exportTo = nullptr);

    void runOscillationTest(int maxTicks = 20);
};
#endif