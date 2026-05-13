/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** APizza
*/

#include "APizza.hpp"

plazza::APizza::APizza(PizzaSize size) : IPizza(size), _size(size)
{
}

plazza::APizza::~APizza()
{
}

PizzaSize plazza::APizza::getSize() const
{
    return _size;
}

std::string plazza::APizza::getSizeString() const
{
    return sizeToString[_size];
}

Ingredients plazza::APizza::getIngredients() const
{
    return _ingredients;
}

std::vector<std::string> plazza::APizza::getIngredientsList() const
{
    std::vector<std::string> listIngredients;

    for (uint16_t bit = 1; bit <= CHIEFLOVE; bit <<= 1) {
        if (_ingredients & bit) {
            listIngredients.push_back(ingredientsToString[static_cast<Ingredients>(bit)]);
        }
    }
    return listIngredients;
}
