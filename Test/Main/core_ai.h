#ifndef CORE_AI_H
#define CORE_AI_H

#include <vector>
#include <string>

extern bool runAI;

class StateStreamTransformer {
public:
    int activationPointCount();
    void load();
    bool webGrader();
    void update();
    void resetWeb();
    int run();
    void runOscillationTest();
};
#endif