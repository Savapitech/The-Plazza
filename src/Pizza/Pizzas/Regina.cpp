#include "Regina.hpp"

plazza::Regina::Regina(PizzaSize size) : APizza(size) {
  _size = size;
  _ingredients =
      static_cast<Ingredients>(DOUGH | TOMATO | GRUYERE | HAM | MUSHROOMS);
  _type = "Regina";
  _bakingTime = 2;
}

plazza::Regina::~Regina() {}
