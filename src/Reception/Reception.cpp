#include <algorithm>
#include <chrono>
#include <poll.h>
#include <sstream>
#include <sys/wait.h>
#include <thread>

#include "Kitchen.hpp"
#include "Logger/Logger.hpp"
#include "Parsing.hpp"
#include "Reception.hpp"
#include "Terminal/Terminal.hpp"

static const char *ingredientName(int idx) {
  static const char *names[plazza::INGREDIENT_COUNT] = {
      "Dough", "Tomato",   "Gruyere",     "Ham",        "Mushrooms",
      "Steak", "Eggplant", "Goat cheese", "Chief love",
  };
  return names[idx];
}

static const char *pizzaTypeName(PizzaType t) {
  switch (t) {
  case ::Regina:
    return "regina";
  case ::Margarita:
    return "margarita";
  case ::Americana:
    return "americana";
  case ::Fantasia:
    return "fantasia";
  }
  return "unknown";
}

static const char *pizzaSizeName(PizzaSize s) {
  switch (s) {
  case ::S:
    return "S";
  case ::M:
    return "M";
  case ::L:
    return "L";
  case ::XL:
    return "XL";
  case ::XXL:
    return "XXL";
  }
  return "?";
}

namespace plazza {

Reception::Reception(float multiplier, int nbCooks, int replenishMs)
    : _multiplier(multiplier), _nbCooks(nbCooks), _replenishMs(replenishMs) {}

Reception::~Reception() {
  for (auto &k : _kitchens) {
    if (!k.closed && k.pid > 0) {
      Message msg{};
      msg.type = MsgType::CLOSE;
      try {
        *k.channel << msg;
      } catch (...) {
      }
    }
  }
  while (!_kitchens.empty()) {
    pollKitchens();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void Reception::spawnKitchen() {
  auto ch = std::make_unique<Channel>();
  pid_t pid = fork();
  if (pid == -1)
    throw std::runtime_error("fork failed");
  if (pid == 0) {
    Terminal::detach();
    ch->childSide();
    {
      Kitchen kitchen(*ch, _nbCooks, _multiplier, _replenishMs);
      kitchen.run();
    }
    _exit(0);
  }
  ch->parentSide();
  _kitchens.emplace_back(pid, std::move(ch));
  LOG_INFO("Spawned kitchen #" + std::to_string(_kitchens.size()));
}

void Reception::dispatchPizza(PizzaType type, PizzaSize size) {
  int capacity = _nbCooks * 2;

  KitchenHandle *best = nullptr;
  for (auto &k : _kitchens) {
    if (!k.closed && k.load < capacity) {
      if (!best || k.load < best->load)
        best = &k;
    }
  }

  if (!best) {
    spawnKitchen();
    best = &_kitchens.back();
  }

  Message msg{};
  msg.type = MsgType::ORDER;
  msg.pizzaType = type;
  msg.pizzaSize = size;
  *best->channel << msg;
  best->load++;
}

void Reception::handleOrder(const std::string &line) {
  auto orders = Parsing::parsingOrder(line);
  for (auto &order : orders) {
    for (size_t i = 0; i < order.getNb(); i++) {
      dispatchPizza(order.getPizzaType(), order.getPizzaSize());
    }
  }
}

void Reception::pollKitchens() {
  for (auto &k : _kitchens) {
    if (k.closed)
      continue;
    Message msg;
    while (k.channel->tryRead(msg)) {
      if (msg.type == MsgType::READY) {
        k.load = std::max(0, k.load - 1);
        LOG_INFO(std::string("[READY] ") + pizzaTypeName(msg.pizzaType) + " " +
                 pizzaSizeName(msg.pizzaSize));
      } else if (msg.type == MsgType::CLOSED) {
        k.closed = true;
        waitpid(k.pid, nullptr, WNOHANG);
        k.pid = -1;
        break;
      } else {
        k.pending.push(msg);
      }
    }
  }
  _kitchens.erase(
      std::remove_if(_kitchens.begin(), _kitchens.end(),
                     [](const KitchenHandle &k) { return k.closed; }),
      _kitchens.end());
}

void Reception::printStatus() {
  if (_kitchens.empty()) {
    LOG_WARN("No kitchens running.");
    return;
  }

  Message req{};
  req.type = MsgType::STATUS_REQ;
  for (auto &k : _kitchens) {
    if (!k.closed)
      *k.channel << req;
  }

  LOG_INFO("Kitchen Status:");
  for (size_t i = 0; i < _kitchens.size(); i++) {
    if (_kitchens[i].closed)
      continue;

    Message res{};
    bool got = false;

    while (!_kitchens[i].pending.empty()) {
      auto &m = _kitchens[i].pending.front();
      if (m.type == MsgType::STATUS_RES) {
        res = m;
        got = true;
      }
      _kitchens[i].pending.pop();
      if (got)
        break;
    }

    if (!got) {
      auto deadline =
          std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
      while (std::chrono::steady_clock::now() < deadline) {
        Message m;
        if (_kitchens[i].channel->tryRead(m)) {
          if (m.type == MsgType::STATUS_RES) {
            res = m;
            got = true;
            break;
          } else if (m.type == MsgType::READY) {
            _kitchens[i].load = std::max(0, _kitchens[i].load - 1);
            LOG_INFO(std::string("[READY] ") + pizzaTypeName(m.pizzaType) +
                     " " + pizzaSizeName(m.pizzaSize));
          } else if (m.type == MsgType::CLOSED) {
            _kitchens[i].closed = true;
            waitpid(_kitchens[i].pid, nullptr, WNOHANG);
            _kitchens[i].pid = -1;
            LOG_WARN("Kitchen #" + std::to_string(i + 1) +
                     " closed unexpectedly during status poll");
            break;
          } else {
            _kitchens[i].pending.push(m);
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
    }

    if (got) {
      std::ostringstream summary;
      summary << "Kitchen #" << i + 1 << ": " << res.cooksTotal << " cooks, "
              << res.cooksBusy << " busy, " << res.pizzasQueued
              << " pizzas in queue";
      LOG_INFO(summary.str());

      std::ostringstream stock;
      stock << "  Stock: ";
      for (int j = 0; j < INGREDIENT_COUNT; j++)
        stock << ingredientName(j) << "=" << res.stock[j] << " ";
      LOG_INFO(stock.str());
    } else {
      LOG_WARN("Kitchen #" + std::to_string(i + 1) +
               " did not respond in time");
    }
  }
}

void Reception::run() {
  Terminal::Guard tg;
  std::string line;
  while (true) {
    pollKitchens();

    struct pollfd pfd{STDIN_FILENO, POLLIN, 0};
    int ret = poll(&pfd, 1, 50);
    if (ret > 0 && (pfd.revents & POLLIN)) {
      auto res = Terminal::readChar(line);
      if (res == Terminal::ReadResult::Eof)
        break;
      if (res == Terminal::ReadResult::Line) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (auto last = line.find_last_not_of(" \t"); last != std::string::npos)
          line.erase(last + 1);
        if (line.empty())
          continue;
        if (line == "status")
          printStatus();
        else
          handleOrder(line);
      }
    } else if (ret < 0) {
      break;
    } else if (ret > 0 && (pfd.revents & POLLHUP)) {
      break;
    }
  }
}

} // namespace plazza
