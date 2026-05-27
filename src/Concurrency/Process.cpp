#include "Process.hpp"

namespace plazza {

Process::~Process() { wait(); }

void Process::start(std::function<void()> fn) {
  if (_pid > 0)
    throw std::runtime_error("Process already running");
  _pid = ::fork();
  if (_pid == -1)
    throw std::runtime_error("fork failed");
  if (_pid == 0) {
    fn();
    ::_exit(0);
  }
}

void Process::wait() {
  if (_pid > 0) {
    ::waitpid(_pid, nullptr, 0);
    _pid = -1;
  }
}

} // namespace plazza
