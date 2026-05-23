#include <chrono>
#include <thread>

#include "Kitchen.hpp"
#include "Logger/Logger.hpp"
#include "PizzaFactory.hpp"
#include <chrono>
#include <thread>

namespace plazza {

static const Ingredients INGREDIENT_LIST[INGREDIENT_COUNT] = {
    DOUGH, TOMATO,   GRUYERE,    HAM,       MUSHROOMS,
    STEAK, EGGPLANT, GOATCHEESE, CHIEFLOVE,
};

Kitchen::Kitchen(Channel &channel, int nbCooks, float multiplier,
                 int replenishMs)
    : _channel(channel), _nbCooks(nbCooks), _multiplier(multiplier),
      _replenishMs(replenishMs), _pool(static_cast<size_t>(nbCooks)),
      _pizzasInFlight(0), _running(true) {
  _stock.fill(INITIAL_STOCK);
  _replenishThread.start(&Kitchen::replenishLoop, this);
}

void Kitchen::replenishLoop() {
  int elapsed = 0;
  while (_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    elapsed += 10;
    if (elapsed >= _replenishMs) {
      elapsed = 0;
      std::unique_lock<std::mutex> lock(_stockMutex.native());
      for (int &s : _stock)
        s++;
    }
  }
}

bool Kitchen::consumeIngredients(Ingredients ing) {
  std::unique_lock<std::mutex> lock(_stockMutex.native());
  for (int i = 0; i < INGREDIENT_COUNT; i++) {
    if ((ing & INGREDIENT_LIST[i]) && _stock[i] == 0)
      return false;
  }
  for (int i = 0; i < INGREDIENT_COUNT; i++) {
    if (ing & INGREDIENT_LIST[i])
      _stock[i]--;
  }
  return true;
}

void Kitchen::cookPizza(PizzaType type, PizzaSize size) {
  auto pizza = PizzaFactory::createPizza(type, size);
  Ingredients needed = pizza->getIngredients();

  while (!consumeIngredients(needed))
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

  size_t ms =
      static_cast<size_t>(pizza->getBakingTime() * _multiplier * 1000.0f);
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));

  {
    std::unique_lock<std::mutex> lock(_loadMutex.native());
    _pizzasInFlight--;
  }

  Message msg{};
  msg.type = MsgType::READY;
  msg.pizzaType = type;
  msg.pizzaSize = size;
  {
    std::unique_lock<std::mutex> lock(_channelMutex.native());
    _channel << msg;
  }
}

void Kitchen::sendStatus() {
  std::array<int, INGREDIENT_COUNT> stock{};
  {
    std::unique_lock<std::mutex> lock(_stockMutex.native());
    stock = _stock;
  }

  Message msg{};
  msg.type = MsgType::STATUS_RES;
  msg.cooksBusy = static_cast<int>(_pool.busy());
  msg.cooksTotal = _nbCooks;
  msg.pizzasQueued = load();
  for (int i = 0; i < INGREDIENT_COUNT; i++)
    msg.stock[i] = stock[i];
  {
    std::unique_lock<std::mutex> lock(_channelMutex.native());
    _channel << msg;
  }
}

int Kitchen::load() const {
  std::unique_lock<std::mutex> lock(_loadMutex.native());
  return _pizzasInFlight;
}

void Kitchen::run() {
  auto idleSince = std::chrono::steady_clock::now();
  bool wasIdle = true;

  while (true) {
    Message msg;
    if (_channel.tryRead(msg)) {
      if (msg.type == MsgType::CLOSE)
        break;
      if (msg.type == MsgType::STATUS_REQ) {
        sendStatus();
      } else if (msg.type == MsgType::ORDER) {
        {
          std::unique_lock<std::mutex> lock(_loadMutex.native());
          _pizzasInFlight++;
        }
        wasIdle = false;
        _pool.enqueue([this, msg] { cookPizza(msg.pizzaType, msg.pizzaSize); });
      }
    }

    int currentLoad = load();
    if (currentLoad == 0) {
      if (!wasIdle) {
        idleSince = std::chrono::steady_clock::now();
        wasIdle = true;
      }
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::steady_clock::now() - idleSince)
                         .count();
      if (elapsed >= 5)
        break;
    } else {
      wasIdle = false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  while (load() > 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  _running = false;
  _replenishThread.join();

  Message closed{};
  closed.type = MsgType::CLOSED;
  {
    std::unique_lock<std::mutex> lock(_channelMutex.native());
    _channel << closed;
  }
}

} // namespace plazza
