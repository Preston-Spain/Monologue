// main.cpp
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include "core_ai.h"
#include "Util.h"

int main() {
    StateStreamTransformer engine;

    std::cout << "Monologue -- (r)un or (t)est? ";
    std::string choice;
    std::getline(std::cin, choice);
    bool isTest = !choice.empty() && (choice[0] == 't' || choice[0] == 'T');

    std::cout << "Max ticks (0 = run forever, Ctrl+C to stop) [default 50]: ";
    std::string ticksInput;
    std::getline(std::cin, ticksInput);
    int maxTicks = 50;
    if (!ticksInput.empty()) {
        try {
            maxTicks = std::stoi(ticksInput);
        } catch (...) {
            std::cout << "Couldn't read that as a number, using 50 ticks." << std::endl;
            maxTicks = 50;
        }
    }

    if (isTest) {
        engine.runOscillationTest(maxTicks > 0 ? maxTicks : 20);
        return 0;
    }

    Util util;
    engine.load(); // reads CSVs, or falls back to a generated default network

    std::vector<float> exportBuffer(engine.activationPointCount(), 0.0f);

    std::cout << "Running";
    if (maxTicks > 0) std::cout << " for " << maxTicks << " ticks";
    else std::cout << " until Ctrl+C";
    std::cout << " (" << exportBuffer.size() << " ActivationPoint(s), printed each tick)...\n";

    int tick = 0;
    while (maxTicks <= 0 || tick < maxTicks) {
        engine.update(tick, &exportBuffer);

        std::cout << "tick " << tick << ": ";
        for (size_t i = 0; i < exportBuffer.size(); i++) {
            std::cout << "AP" << i << "=" << exportBuffer[i] << " ";
        }
        std::cout << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow enough to actually read
        tick++;
    }

    std::cout << "Done (" << tick << " ticks)." << std::endl;
    return 0;
}