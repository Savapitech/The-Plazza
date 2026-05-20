/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** APizza
*/

#ifndef APIZZA_HPP_
#define APIZZA_HPP_

#include "IPizza.hpp"

namespace plazza {
class APizza : public plazza::IPizza {
public:
  APizza(PizzaSize size);
  ~APizza();

  PizzaSize getSize() const override;
  std::string getSizeString() const override;

  Ingredients getIngredients() const override;
  std::vector<std::string> getIngredientsList() const override;

  std::string getType() const override;
  size_t getBakingTime() const override;

protected:
  PizzaSize _size;
  Ingredients _ingredients;
  std::string _type;
  size_t _bakingTime;

private:
};
} // namespace plazza

#endif /* !APIZZA_HPP_ */
