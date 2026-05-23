#pragma once

#include "Channel.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "ThreadPool.hpp"

#include <array>
#include <atomic>
#include <chrono>

namespace plazza {

static constexpr int INITIAL_STOCK = 5;

class Kitchen {
public:
  Kitchen(Channel &channel, int nbCooks, float multiplier, int replenishMs);
  ~Kitchen() = default;

  void run();

private:
  void cookPizza(PizzaType type, PizzaSize size);
  void replenishLoop();
  bool consumeIngredients(Ingredients ing);
  void sendStatus();
  int load() const;

  Channel &_channel;
  int _nbCooks;
  float _multiplier;
  int _replenishMs;

  ThreadPool _pool;
  mutable Mutex _stockMutex;
  std::array<int, INGREDIENT_COUNT> _stock;
  mutable Mutex _loadMutex;
  int _pizzasInFlight;
  Mutex _channelMutex;

  std::atomic<bool> _running;
  Thread _replenishThread;
};

} // namespace plazza
