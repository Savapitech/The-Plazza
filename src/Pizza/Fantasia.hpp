/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Fantasia
*/

#ifndef FANTASIA_HPP_
#define FANTASIA_HPP_

#include "IPizza.hpp"

namespace plazza {
    class Fantasia : public IPizza {
        public:
            Fantasia(PizzaSize size);
            ~Fantasia();

        protected:
        private:
            PizzaSize _size;
    };
}

#endif /* !FANTASIA_HPP_ */
