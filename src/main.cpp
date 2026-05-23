#include <iostream>

#include "Logger/Logger.hpp"
#include "Reception.hpp"

static void usage(const char *prog) {
    std::cerr << "Usage: " << prog
              << " multiplier nb_cooks ingredient_replenish_time" << std::endl;
}

int main(int argc, char **argv) {
    std::cout << "The Plazza, the best pizzeria" << std::endl;

    if (argc != 4) {
        usage(argv[0]);
        return 84;
    }

    float multiplier;
    int nbCooks;
    int replenishMs;

    try {
        multiplier = std::stof(argv[1]);
        nbCooks = std::stoi(argv[2]);
        replenishMs = std::stoi(argv[3]);
    } catch (const std::exception &) {
        usage(argv[0]);
        return 84;
    }

    if (multiplier <= 0.0f || nbCooks <= 0 || replenishMs <= 0) {
        usage(argv[0]);
        return 84;
    }

    Logger::setLevel(L_LOG);

    try {
        plazza::Reception reception(multiplier, nbCooks, replenishMs);
        reception.run();
    } catch (const std::exception &e) {
        LOG_FATAL(e.what());
        return 84;
    }

    return 0;
}
