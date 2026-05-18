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

plazza::ThreadPool::ThreadPool()
{
    for (int i = 0; i < NB_COOKS; i++)
        _cooks.emplace_back([this]{cooking();});
}

void plazza::ThreadPool::addPizza(std::unique_ptr<IPizza> pizza)
{
    std::lock_guard lock(_mtx);

    _tasks.push_back(std::move(pizza));
    _cv.notify_one();
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
        lock.unlock();
        std::cout << "Cooking " << pizza->getSizeString() << std::endl;
        //sleep for the cooking time
    }
}
