/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaFactory
*/

#ifndef PIZZAFACTORY_HPP_
#define PIZZAFACTORY_HPP_

#include "IPizza.hpp"
#include "Americana.hpp"
#include "Fantasia.hpp"
#include "Margarita.hpp"
#include "Regina.hpp"

namespace plazza
{
    class PizzaFactory {
        public:

        std::unique_ptr<plazza::IPizza> createPizza(PizzaType type, PizzaSize size);

        protected:
        private:
            std::unique_ptr<plazza::IPizza> createAmericana(PizzaSize size);
            std::unique_ptr<plazza::IPizza> createFantasia(PizzaSize size);
            std::unique_ptr<plazza::IPizza> createMargarita(PizzaSize size);
            std::unique_ptr<plazza::IPizza> createRegina(PizzaSize size);
    };
} // namespace plazza

#endif /* !PIZZAFACTORY_HPP_ */
