#pragma once

#include <condition_variable>
#include <mutex>

namespace plazza {

class CondVar {
public:
    CondVar() = default;
    ~CondVar() = default;

    CondVar(const CondVar &) = delete;
    CondVar &operator=(const CondVar &) = delete;

    void wait(std::unique_lock<std::mutex> &lock) { _cv.wait(lock); }

    template <typename Pred>
    void wait(std::unique_lock<std::mutex> &lock, Pred pred) {
        _cv.wait(lock, pred);
    }

    void notifyOne() { _cv.notify_one(); }
    void notifyAll() { _cv.notify_all(); }

private:
    std::condition_variable _cv;
};

} // namespace plazza
