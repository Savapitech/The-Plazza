/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Parsing
*/

#ifndef PARSING_HPP_
#define PARSING_HPP_

#include <map>

#include "Order.hpp"

static inline std::map<std::string, PizzaType> stringToPizzaType = {
    {"americana", Americana},
    {"fantasia", Fantasia},
    {"margarita", Margarita},
    {"regina", Regina}};

static inline std::map<std::string, PizzaSize> stringToPizzaSize = {
    {"S", PizzaSize::S},
    {"M", PizzaSize::M},
    {"L", PizzaSize::L},
    {"XL", PizzaSize::XL},
    {"XXL", PizzaSize::XXL}};

namespace plazza {
class Parsing {
public:
  static std::vector<plazza::Order> parsingOrder(std::string orderLine);

protected:
private:
};
} // namespace plazza

#endif /* !PARSING_HPP_ */
