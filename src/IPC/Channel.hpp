#pragma once

#include <poll.h>

#include "IPizza.hpp"

namespace plazza {

static constexpr int INGREDIENT_COUNT = 9;

enum class MsgType {
  ORDER = 1,
  READY = 2,
  STATUS_REQ = 3,
  STATUS_RES = 4,
  CLOSE = 5,
  CLOSED = 6,
};

struct Message {
  MsgType type;
  PizzaType pizzaType;
  PizzaSize pizzaSize;
  int cooksBusy;
  int cooksTotal;
  int pizzasQueued;
  int stock[INGREDIENT_COUNT];
};

class Channel {
public:
  Channel();
  ~Channel();

  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &) = delete;

  void parentSide();
  void childSide();

  Channel &operator<<(const Message &msg);
  Channel &operator>>(Message &msg);
  bool tryRead(Message &msg);

private:
  int _toChild[2];
  int _toParent[2];
  int _rfd;
  int _wfd;

  static void closeFd(int &fd);
};

} // namespace plazza
