/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Regina
*/

#ifndef REGINA_HPP_
#define REGINA_HPP_

#include "IPizza.hpp"

namespace plazza
{
    class Regina : public IPizza {
        public:
            Regina(PizzaSize size);
            ~Regina();

        protected:
        private:
            PizzaSize _size;
    };
} // namespace plazza

#endif /* !REGINA_HPP_ */
