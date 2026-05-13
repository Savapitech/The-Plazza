/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** IPizza
*/

#ifndef IPIZZA_HPP_
#define IPIZZA_HPP_

#include <cstdint>
#include <iomanip>
#include <map>
#include <memory>
#include <vector>

enum Ingredients : uint16_t {
  AUCUN = 0,
  DOUGH = 1 << 0,
  TOMATO = 1 << 1,
  GRUYERE = 1 << 2,
  HAM = 1 << 3,
  MUSHROOMS = 1 << 4,
  STEAK = 1 << 5,
  EGGPLANT = 1 << 6,
  GOATCHEESE = 1 << 7,
  CHIEFLOVE = 1 << 8,
};

enum PizzaType { Regina = 1, Margarita = 2, Americana = 4, Fantasia = 8 };

enum PizzaSize { S = 1, M = 2, L = 4, XL = 8, XXL = 16 };

static inline std::map<PizzaSize, std::string> sizeToString = {
    {PizzaSize::S, "S"},
    {PizzaSize::M, "M"},
    {PizzaSize::L, "L"},
    {PizzaSize::XL, "XL"},
    {PizzaSize::XXL, "XXL"}};

static inline std::map<Ingredients, std::string> ingredientsToString = {
    {DOUGH, "Dough"},         {TOMATO, "Tomato"},
    {GRUYERE, "Gruyere"},     {HAM, "Ham"},
    {MUSHROOMS, "Mushrooms"}, {STEAK, "Steak"},
    {EGGPLANT, "Eggplant"},   {GOATCHEESE, "Goat cheese"},
    {CHIEFLOVE, "Chief love"}};

namespace plazza {
class IPizza {
public:
  IPizza(PizzaSize size){};
  virtual ~IPizza() = default;

  virtual PizzaSize getSize() const = 0;
  virtual std::string getSizeString() const = 0;

  virtual Ingredients getIngredients() const = 0;
  virtual std::vector<std::string> getIngredientsList() const = 0;

protected:
private:
};
} // namespace plazza

#endif /* !IPIZZA_HPP_ */
