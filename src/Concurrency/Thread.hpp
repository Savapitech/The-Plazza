#pragma once

#include <thread>

namespace plazza {

class Thread {
public:
  Thread() = default;
  ~Thread();

  Thread(const Thread &) = delete;
  Thread &operator=(const Thread &) = delete;
  Thread(Thread &&) = default;
  Thread &operator=(Thread &&) = default;

  template <typename F, typename... Args> void start(F &&f, Args &&...args) {
    _t = std::thread(std::forward<F>(f), std::forward<Args>(args)...);
  }

  void join();
  bool joinable() const;

private:
  std::thread _t;
};

} // namespace plazza
