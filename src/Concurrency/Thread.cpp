#include "Thread.hpp"

namespace plazza {

Thread::~Thread() {
    if (_t.joinable())
        _t.join();
}

void Thread::join() {
    if (_t.joinable())
        _t.join();
}

bool Thread::joinable() const {
    return _t.joinable();
}

} // namespace plazza
