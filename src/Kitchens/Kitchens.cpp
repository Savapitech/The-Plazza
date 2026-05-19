/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, May, 2026                                                     *
 * Title           - Plazza                                                             *
 * Description     -                                                                    *
 *     Kitchens                                                                         *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *         ░        ░       ░░        ░        ░        ░░      ░░  ░░░░  ░             *
 *         ▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒  ▒▒▒▒  ▒             *
 *         ▓      ▓▓▓       ▓▓▓▓▓  ▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓  ▓▓▓▓▓▓▓        ▓             *
 *         █  ███████  ██████████  ███████  ████  ███████  ████  █  ████  █             *
 *         █        █  ███████        ████  ████        ██      ██  ████  █             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "Kitchens.hpp"

plazza::Kitchens::Kitchens(int nbCooks)
{
    _run = true;
    _lastActivity = std::chrono::steady_clock::now();
    _threadPool = ThreadPool(nbCooks);
}

void plazza::Kitchens::setOrders(std::vector<std::unique_ptr<IPizza>> orders)
{
    for (auto &order : orders)
        _threadPool.addPizza(std::move(order));
}

void plazza::Kitchens::setStock(Ingredients stock)
{
    _stock = stock;
}

void plazza::Kitchens::run()
{
    while (_run) {
        if (_threadPool.getTasksCount() == 0 && _threadPool.getActiveCooksCount() == 0 && std::chrono::steady_clock::now() - _lastActivity > std::chrono::seconds(5))
                _run = false;
        else
            _lastActivity = std::chrono::steady_clock::now();
    }
}
