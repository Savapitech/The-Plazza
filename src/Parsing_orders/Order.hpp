/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Order
*/

#ifndef ORDER_HPP_
#define ORDER_HPP_

#include "IPizza.hpp"
#include <cstdio>
#include <iomanip>
#include <memory>
#include <string>

namespace plazza {
class Order {
public:
  Order(size_t nb, std::unique_ptr<plazza::IPizza> pizza);
  // move constructor
  Order(Order &&) = default;
  ~Order();

  size_t getNb();
  const plazza::IPizza &getPizza() const;

protected:
  size_t _nb;
  std::unique_ptr<plazza::IPizza> _pizza;

private:
};
} // namespace plazza

#endif /* !ORDER_HPP_ */
