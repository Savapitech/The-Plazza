/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Order
*/

#ifndef ORDER_HPP_
#define ORDER_HPP_

#include <cstdio>
#include <iomanip>
#include <memory>
#include <string>

#include "IPizza.hpp"

namespace plazza {
class Order {
public:
  Order(size_t nb, PizzaType type, PizzaSize size,
        std::unique_ptr<plazza::IPizza> pizza);
  Order(Order &&) = default;
  ~Order();

  size_t getNb() const;
  PizzaType getPizzaType() const;
  PizzaSize getPizzaSize() const;
  const plazza::IPizza &getPizza() const;

protected:
  size_t _nb;
  PizzaType _pizzaType;
  PizzaSize _pizzaSize;
  std::unique_ptr<plazza::IPizza> _pizza;

private:
};
} // namespace plazza

#endif /* !ORDER_HPP_ */
