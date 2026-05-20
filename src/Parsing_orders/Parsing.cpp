/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Parsing
*/

#include "Parsing.hpp"

std::vector<std::string> orderLineToArray(std::string orderLine) {
  std::vector<std::string> arrayOrders;
  std::stringstream ss(orderLine);
  std::string tmp;

  while (std::getline(ss, tmp, ';')) {
    arrayOrders.push_back(tmp);
  }
  return arrayOrders;
}

std::vector<std::string> decompositionOrder(std::string order) {
  std::vector<std::string> sectionOrder;
  std::stringstream ss(order);
  std::string tmp;

  while (std::getline(ss, tmp, ' ')) {
    if (!tmp.empty()) {
      sectionOrder.push_back(tmp);
    }
  }
  return sectionOrder;
}

bool isDigitString(std::string string) {
  for (size_t i = 0; i < string.size(); i++) {
    if (isdigit(string[i]) == 0) {
      return false;
    }
  }
  return true;
}

std::vector<plazza::Order>
plazza::Parsing::parsingOrder(std::string orderLine) {
  std::vector<std::string> arrayOrders = orderLineToArray(orderLine);
  std::vector<plazza::Order> orders;
  std::vector<std::string> sectionOrder;

  for (size_t i = 0; i < arrayOrders.size(); i++) {
    sectionOrder = decompositionOrder(arrayOrders[i]);
    if (sectionOrder.size() != 3) {
      std::cout << "Not enough parameters for the order n°" << i + 1 << " !"
                << std::endl;
      continue;
    }
    std::cout << "type: [" << sectionOrder[0] << "] size: [" << sectionOrder[1]
              << "] number: [" << sectionOrder[2] << "]" << std::endl;
    if (sectionOrder[0] == "americana" || sectionOrder[0] == "fantasia" ||
        sectionOrder[0] == "margarita" || sectionOrder[0] == "regina") {
      if (sectionOrder[1] == "S" || sectionOrder[1] == "L" ||
          sectionOrder[1] == "M" || sectionOrder[1] == "XL" ||
          sectionOrder[1] == "XXL") {
        sectionOrder[2].erase(0, 1);
        if (isDigitString(sectionOrder[2]) == true) {
          if (stoi(sectionOrder[2]) > 0) {
            auto pizza = plazza::PizzaFactory::createPizza(
                stringToPizzaType[static_cast<std::string>(sectionOrder[0])],
                stringToPizzaSize[static_cast<std::string>(sectionOrder[1])]);
            orders.push_back(Order(stoi(sectionOrder[2]), std::move(pizza)));
          }
        } else {
          std::cout << "This order is not acceptable(number of pizza) : ["
                    << arrayOrders[i] << "] !" << std::endl;
          continue;
        }
      } else {
        std::cout << "This order is not acceptable(size of pizza): ["
                  << arrayOrders[i] << "] !" << std::endl;
        continue;
      }
    } else {
      std::cout << "This order is not acceptable(type of pizza): ["
                << arrayOrders[i] << "] !" << std::endl;
      continue;
    }
  }
  return orders;
}
