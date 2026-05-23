/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Order
*/

#include "Order.hpp"

plazza::Order::Order(size_t nb, PizzaType type, PizzaSize size,
                     std::unique_ptr<plazza::IPizza> pizza)
    : _nb(nb), _pizzaType(type), _pizzaSize(size), _pizza(std::move(pizza)) {}

plazza::Order::~Order() {}

size_t plazza::Order::getNb() const { return _nb; }

PizzaType plazza::Order::getPizzaType() const { return _pizzaType; }

PizzaSize plazza::Order::getPizzaSize() const { return _pizzaSize; }

const plazza::IPizza &plazza::Order::getPizza() const { return *_pizza; }
