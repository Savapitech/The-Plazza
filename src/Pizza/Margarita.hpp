/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Margarita
*/

#ifndef MARGARITA_HPP_
#define MARGARITA_HPP_

#include "IPizza.hpp"

namespace plazza
{
    class Margarita : public IPizza {
        public:
            Margarita(PizzaSize size);
            ~Margarita();

        protected:
        private:
            PizzaSize _size;
    };
} // namespace plazza




#endif /* !MARGARITA_HPP_ */
