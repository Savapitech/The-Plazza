/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Parsing
*/

#include <sstream>

#include "Parsing.hpp"

static std::vector<std::string> splitBy(const std::string &s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delim)) {
        parts.push_back(token);
    }
    return parts;
}

static std::vector<std::string> splitWords(const std::string &s) {
    std::vector<std::string> words;
    std::stringstream ss(s);
    std::string word;
    while (ss >> word) {
        words.push_back(word);
    }
    return words;
}

static bool validQuantity(const std::string &s) {
    if (s.size() < 2 || s[0] != 'x')
        return false;
    for (size_t i = 1; i < s.size(); i++) {
        if (!isdigit(s[i]))
            return false;
    }
    return stoi(s.substr(1)) > 0;
}

std::vector<plazza::Order>
plazza::Parsing::parsingOrder(std::string orderLine) {
    std::vector<plazza::Order> orders;

    for (auto &chunk : splitBy(orderLine, ';')) {
        auto words = splitWords(chunk);
        if (words.size() != 3) {
            std::cerr << "Invalid order: [" << chunk << "]" << std::endl;
            continue;
        }

        auto itType = stringToPizzaType.find(words[0]);
        if (itType == stringToPizzaType.end()) {
            std::cerr << "Unknown pizza type: [" << words[0] << "]" << std::endl;
            continue;
        }

        auto itSize = stringToPizzaSize.find(words[1]);
        if (itSize == stringToPizzaSize.end()) {
            std::cerr << "Unknown pizza size: [" << words[1] << "]" << std::endl;
            continue;
        }

        if (!validQuantity(words[2])) {
            std::cerr << "Invalid quantity: [" << words[2] << "]" << std::endl;
            continue;
        }

        PizzaType type = itType->second;
        PizzaSize size = itSize->second;
        int nb = stoi(words[2].substr(1));
        auto pizza = PizzaFactory::createPizza(type, size);
        orders.emplace_back(static_cast<size_t>(nb), type, size,
                            std::move(pizza));
    }
    return orders;
}
