#include "PizzaFactory.hpp"
#include <iostream>

int main(void) {
  std::cout << "The Plazza, the best pizzeria" << std::endl;

  auto americana =
      plazza::PizzaFactory::createPizza(PizzaType::Americana, PizzaSize::M);

  std::cout << "Americana, size: [" << americana->getSize()
            << "], ingredients: [" << americana->getIngredients() << "] !"
            << std::endl;
  std::cout << "or..." << std::endl;
  std::cout << "Americana, size: [" << americana->getSizeString()
            << "], ingredients: [";
  std::vector<std::string> listIngredients = americana->getIngredientsList();
  for (size_t i = 0; i < listIngredients.size(); i++) {
    std::cout << listIngredients[i] << ",";
  }
  std::cout << "] !" << std::endl;
}
