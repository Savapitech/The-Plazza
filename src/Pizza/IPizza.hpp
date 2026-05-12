/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** IPizza
*/

#ifndef IPIZZA_HPP_
#define IPIZZA_HPP_

#include <cstdint>
#include <memory>
#include <iomanip>

enum Ingredients : uint16_t {
    AUCUN = 0,
    DOUGH = 1 << 0,
    TOMATO = 1 << 1,
    GRUYERE = 1 << 2,
    HAM = 1 << 3,
    MUSHROOMS = 1 << 4,
    STEAK = 1 << 5,
    EGGPLANT = 1 << 6,
    GOATCHEESE = 1 << 7,
    CHIEFLOVE = 1 << 8,
};

enum PizzaType {
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8
};

enum PizzaSize {
    S = 1,
    M = 2,
    L = 4,
    XL = 8,
    XXL = 16
};

namespace plazza {
    class IPizza {
        public:
            IPizza(PizzaSize size);
            virtual ~IPizza() = default;
        protected:
        private:
    };
}

#endif /* !IPIZZA_HPP_ */
