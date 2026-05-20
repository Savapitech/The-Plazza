/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Order
*/

#include "Order.hpp"

plazza::Order::Order(size_t nb, std::unique_ptr<plazza::IPizza> pizza)
    : _nb(nb), _pizza(std::move(pizza)) {}

plazza::Order::~Order() {}

size_t plazza::Order::getNb() { return _nb; }

const plazza::IPizza &plazza::Order::getPizza() const { return *_pizza; }
