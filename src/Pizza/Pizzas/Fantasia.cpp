#include "Fantasia.hpp"

plazza::Fantasia::Fantasia(PizzaSize size) : APizza(size) {
  _size = size;
  _ingredients = static_cast<Ingredients>(DOUGH | TOMATO | EGGPLANT |
                                          GOATCHEESE | CHIEFLOVE);
}

plazza::Fantasia::~Fantasia() {}
