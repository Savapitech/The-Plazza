/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Americana
*/

#include "Americana.hpp"

plazza::Americana::Americana(PizzaSize size) : IPizza(size)
{
    _size = size;
    _ingredients = static_cast<Ingredients>(DOUGH | TOMATO | GRUYERE | STEAK);
}

plazza::Americana::~Americana()
{
}
