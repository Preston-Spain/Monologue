// main.cpp
#include <vector>
#include <iostream>
#include "core_ai.h"
#include "Util.h"

int main() {
    bool runAI = true;
    StateStreamTransformer engine;
    Util util;
    // util.runAI = true;

    engine.load(); // reads CSVs, or falls back to a generated default network

    std::vector<float> exportBuffer(engine.activationPointCount(), 0.0f);

    try {
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "run() failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}