/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaFactory
*/

#include "PizzaFactory.hpp"

std::unique_ptr<plazza::IPizza> plazza::PizzaFactory::createPizza(PizzaType type, PizzaSize size)
{
    switch (type)
    {
    case PizzaType::Americana:
        return createAmericana(size);
        break;
    case PizzaType::Fantasia:
        return createFantasia(size);
        break;
    case PizzaType::Margarita:
        return createMargarita(size);
        break;
    case PizzaType::Regina:
        return createRegina(size);
        break;
    default:
        return nullptr;
        break;
    }
}

std::unique_ptr<plazza::IPizza> plazza::PizzaFactory::createAmericana(PizzaSize size)
{
    return std::make_unique<plazza::Americana>(size);
}

std::unique_ptr<plazza::IPizza> plazza::PizzaFactory::createFantasia(PizzaSize size)
{
    return std::make_unique<plazza::Fantasia>(size);
}

std::unique_ptr<plazza::IPizza> plazza::PizzaFactory::createMargarita(PizzaSize size)
{
    return std::make_unique<plazza::Margarita>(size);
}

std::unique_ptr<plazza::IPizza> plazza::PizzaFactory::createRegina(PizzaSize size)
{
    return std::make_unique<plazza::Regina>(size);
}
