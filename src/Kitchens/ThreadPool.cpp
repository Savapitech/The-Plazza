/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, May, 2026                                                     *
 * Title           - Plazza                                                             *
 * Description     -                                                                    *
 *     ThreadPool                                                                       *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       _|_|_|_|  _|_|_|    _|_|_|  _|_|_|_|_|  _|_|_|_|    _|_|_|  _|    _|           *
 *       _|        _|    _|    _|        _|      _|        _|        _|    _|           *
 *       _|_|_|    _|_|_|      _|        _|      _|_|_|    _|        _|_|_|_|           *
 *       _|        _|          _|        _|      _|        _|        _|    _|           *
 *       _|_|_|_|  _|        _|_|_|      _|      _|_|_|_|    _|_|_|  _|    _|           *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "ThreadPool.hpp"

plazza::ThreadPool::ThreadPool(int nbCooks)
{
    _stop = false;
    _nbCooks = nbCooks;
    _activeCooks = 0;
    for (int i = 0; i < _nbCooks; i++)
        _cooks.emplace_back([this]{cooking();});
}

void plazza::ThreadPool::addPizza(std::unique_ptr<IPizza> pizza)
{
    std::lock_guard lock(_mtx);

    _tasks.push_back(std::move(pizza));
    _cv.notify_one();
}

plazza::ThreadPool &plazza::ThreadPool::operator=(ThreadPool const &)
{
    return *this;
}

void plazza::ThreadPool::cooking()
{
    while(!_stop) {
        std::unique_lock lock(_mtx);
        _cv.wait(lock, [this]{return !_tasks.empty() || _stop;});
        if (_stop)
            return;
        std::unique_ptr<IPizza> pizza = std::move(_tasks.back());
        _tasks.pop_back();
        _activeCooks++;
        lock.unlock();
        std::cout << "Cooking " << pizza->getSizeString() << std::endl;
        //sleep for the cooking time
        std::lock_guard lock2(_mtx);
        _activeCooks--;
        _cv.notify_all();
    }
}

std::size_t plazza::ThreadPool::getTasksCount() const
{
    std::lock_guard lock(_mtx);
    return _tasks.size();
}

std::size_t plazza::ThreadPool::getActiveCooksCount() const
{
    std::lock_guard lock(_mtx);
    return _activeCooks;
}

plazza::ThreadPool::~ThreadPool()
{
    _stop = true;
    _cv.notify_all();
    for (std::thread &cook : _cooks)
        cook.join();
}
