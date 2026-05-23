#pragma once

#include <atomic>
#include <functional>
#include <queue>
#include <vector>

#include "CondVar.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace plazza {

class ThreadPool {
public:
    explicit ThreadPool(size_t n);
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    void enqueue(std::function<void()> task);
    size_t busy() const;
    size_t size() const;
    size_t pending() const;

private:
    void workerLoop();

    std::vector<Thread> _workers;
    std::queue<std::function<void()>> _tasks;
    mutable Mutex _mutex;
    CondVar _cond;
    std::atomic<bool> _stop;
    size_t _busyCount;
};

} // namespace plazza
