#pragma once

#include <mutex>

namespace plazza {

class Mutex {
public:
  Mutex() = default;
  ~Mutex() = default;

  Mutex(const Mutex &) = delete;
  Mutex &operator=(const Mutex &) = delete;

  void lock() { _m.lock(); }
  void unlock() { _m.unlock(); }
  std::mutex &native() { return _m; }
  const std::mutex &native() const { return _m; }

private:
  std::mutex _m;
};

} // namespace plazza
