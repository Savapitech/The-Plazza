#include "ThreadPool.hpp"

namespace plazza {

ThreadPool::ThreadPool(size_t n) : _stop(false), _busyCount(0) {
    for (size_t i = 0; i < n; ++i) {
        _workers.emplace_back();
        _workers.back().start(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    _stop.store(true);
    _cond.notifyAll();
    for (auto &w : _workers)
        w.join();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(_mutex.native());
        _tasks.push(std::move(task));
    }
    _cond.notifyOne();
}

size_t ThreadPool::busy() const {
    std::unique_lock<std::mutex> lock(_mutex.native());
    return _busyCount;
}

size_t ThreadPool::size() const {
    return _workers.size();
}

size_t ThreadPool::pending() const {
    std::unique_lock<std::mutex> lock(_mutex.native());
    return _tasks.size();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex.native());
            _cond.wait(lock, [this] { return _stop.load() || !_tasks.empty(); });
            if (_stop.load() && _tasks.empty())
                return;
            task = std::move(_tasks.front());
            _tasks.pop();
            ++_busyCount;
        }
        task();
        {
            std::unique_lock<std::mutex> lock(_mutex.native());
            --_busyCount;
        }
        _cond.notifyAll();
    }
}

} // namespace plazza
