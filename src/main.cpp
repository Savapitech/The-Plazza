#include "Parsing.hpp"
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

  std::cout << "Let's order !" << std::endl;
  std::string line;
  std::vector<plazza::Order> order;
  while (1) {
    getline(std::cin, line);
    order = plazza::Parsing::parsingOrder(line);
    std::cout << "Get : " << std::endl;
    for (size_t i = 0; i < order.size(); i++) {
      std::cout << "---------" << std::endl;
      std::cout << order[i].getPizza().getType() << std::endl;
      std::cout << order[i].getNb() << std::endl;
      std::cout << "---------" << std::endl;
    }
  }
}
