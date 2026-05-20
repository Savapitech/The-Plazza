#include "Margarita.hpp"

plazza::Margarita::Margarita(PizzaSize size) : APizza(size) {
  _size = size;
  _ingredients = static_cast<Ingredients>(DOUGH | TOMATO | GRUYERE);
  _type = "Margarita";
  _bakingTime = 1;
}

plazza::Margarita::~Margarita() {}
