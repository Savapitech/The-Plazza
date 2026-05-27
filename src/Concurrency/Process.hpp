#pragma once

#include <functional>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

namespace plazza {

class Process {
public:
  Process() = default;
  ~Process();

  Process(const Process &) = delete;
  Process &operator=(const Process &) = delete;
  Process(Process &&other) noexcept : _pid(other._pid) { other._pid = -1; }
  Process &operator=(Process &&other) noexcept {
    if (this != &other) {
      wait();
      _pid = other._pid;
      other._pid = -1;
    }
    return *this;
  }

  void start(std::function<void()> fn);
  void wait();
  bool running() const { return _pid > 0; }
  pid_t pid() const { return _pid; }

private:
  pid_t _pid = -1;
};

} // namespace plazza
