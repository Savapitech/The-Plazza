#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <unistd.h>

#include "Channel.hpp"

namespace plazza {

struct KitchenHandle {
    pid_t pid;
    std::unique_ptr<Channel> channel;
    int load;
    bool closed;
    std::queue<Message> pending;

    KitchenHandle(pid_t p, std::unique_ptr<Channel> ch)
        : pid(p), channel(std::move(ch)), load(0), closed(false) {}

    KitchenHandle(const KitchenHandle &) = delete;
    KitchenHandle &operator=(const KitchenHandle &) = delete;
    KitchenHandle(KitchenHandle &&) = default;
    KitchenHandle &operator=(KitchenHandle &&) = default;
};

class Reception {
public:
    Reception(float multiplier, int nbCooks, int replenishMs);
    ~Reception();

    void run();

private:
    void handleOrder(const std::string &line);
    void dispatchPizza(PizzaType type, PizzaSize size);
    void spawnKitchen();
    void pollKitchens();
    void printStatus();

    float _multiplier;
    int _nbCooks;
    int _replenishMs;
    std::vector<KitchenHandle> _kitchens;
};

} // namespace plazza
