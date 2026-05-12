/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Americana
*/

#ifndef AMERICANA_HPP_
#define AMERICANA_HPP_

#include "IPizza.hpp"

namespace plazza {
    class Americana : public IPizza {
        public:
            Americana(PizzaSize size);
            ~Americana();

        protected:
        private:
            PizzaSize _size;
            Ingredients _ingredients;
    };
}

#endif /* !AMERICANA_HPP_ */
