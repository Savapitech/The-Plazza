/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** unit tests
*/

#include "Parsing.hpp"
#include "PizzaFactory.hpp"
#include "ThreadPool.hpp"
#include <atomic>
#include <chrono>
#include <criterion/criterion.h>
#include <thread>

Test(parsing, valid_margarita_s_x1) {
  auto orders = plazza::Parsing::parsingOrder("margarita S x1");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getNb(), 1u);
  cr_assert_eq(orders[0].getPizzaType(), Margarita);
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::S);
}

Test(parsing, valid_americana_m_x2) {
  auto orders = plazza::Parsing::parsingOrder("americana M x2");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getNb(), 2u);
  cr_assert_eq(orders[0].getPizzaType(), Americana);
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::M);
}

Test(parsing, valid_regina_xl_x1) {
  auto orders = plazza::Parsing::parsingOrder("regina XL x1");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getPizzaType(), Regina);
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::XL);
}

Test(parsing, valid_fantasia_xxl_x3) {
  auto orders = plazza::Parsing::parsingOrder("fantasia XXL x3");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getPizzaType(), Fantasia);
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::XXL);
  cr_assert_eq(orders[0].getNb(), 3u);
}

Test(parsing, valid_all_sizes) {
  PizzaSize sizes[] = {S, M, L, XL, XXL};
  const char *snames[] = {"S", "M", "L", "XL", "XXL"};
  for (int i = 0; i < 5; i++) {
    std::string order = std::string("americana ") + snames[i] + " x1";
    auto orders = plazza::Parsing::parsingOrder(order);
    cr_assert_eq(orders.size(), 1u);
    cr_assert_eq(orders[0].getPizzaSize(), sizes[i]);
  }
}

Test(parsing, empty_string_returns_empty) {
  auto orders = plazza::Parsing::parsingOrder("");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, invalid_type_returns_empty) {
  auto orders = plazza::Parsing::parsingOrder("hawaiian M x1");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, invalid_size_returns_empty) {
  auto orders = plazza::Parsing::parsingOrder("americana XXXL x1");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, invalid_qty_no_x_prefix) {
  auto orders = plazza::Parsing::parsingOrder("americana M 2");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, invalid_qty_x0) {
  auto orders = plazza::Parsing::parsingOrder("americana M x0");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, invalid_qty_bare_x) {
  auto orders = plazza::Parsing::parsingOrder("americana M x");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, multi_order_two_pizzas) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1; margarita S x2");
  cr_assert_eq(orders.size(), 2u);
}

Test(parsing, multi_order_counts_correct) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1; margarita S x2");
  cr_assert_eq(orders[0].getNb(), 1u);
  cr_assert_eq(orders[1].getNb(), 2u);
}

Test(parsing, multi_order_types_correct) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1; margarita S x1");
  cr_assert_eq(orders[0].getPizzaType(), Americana);
  cr_assert_eq(orders[1].getPizzaType(), Margarita);
}

Test(parsing, invalid_in_multi_skipped) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1; badpizza S x1");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getPizzaType(), Americana);
}

Test(parsing, extra_spaces_handled) {
  auto orders = plazza::Parsing::parsingOrder("  americana  M  x1  ");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getPizzaType(), Americana);
}

Test(parsing, trailing_semicolon) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1;");
  cr_assert_eq(orders.size(), 1u);
}

Test(parsing, only_semicolons) {
  auto orders = plazza::Parsing::parsingOrder(";;;");
  cr_assert_eq(orders.size(), 0u);
}

Test(parsing, large_quantity) {
  auto orders = plazza::Parsing::parsingOrder("margarita S x99");
  cr_assert_eq(orders.size(), 1u);
  cr_assert_eq(orders[0].getNb(), 99u);
}

Test(parsing, four_orders_in_line) {
  auto orders = plazza::Parsing::parsingOrder(
      "americana M x1; margarita S x1; regina L x1; fantasia XL x1");
  cr_assert_eq(orders.size(), 4u);
}

Test(parsing, missing_size_token) {
  auto orders = plazza::Parsing::parsingOrder("americana x1");
  cr_assert_eq(orders.size(), 0u);
}

Test(order, get_nb) {
  auto orders = plazza::Parsing::parsingOrder("americana M x5");
  cr_assert_eq(orders[0].getNb(), 5u);
}

Test(order, get_pizza_type) {
  auto orders = plazza::Parsing::parsingOrder("fantasia S x1");
  cr_assert_eq(orders[0].getPizzaType(), Fantasia);
}

Test(order, get_pizza_size) {
  auto orders = plazza::Parsing::parsingOrder("regina XXL x1");
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::XXL);
}

Test(order, get_pizza_ref_valid) {
  auto orders = plazza::Parsing::parsingOrder("margarita M x1");
  cr_assert_eq(orders[0].getPizza().getType(), "Margarita");
}

Test(order, nb_one) {
  auto orders = plazza::Parsing::parsingOrder("americana M x1");
  cr_assert_eq(orders[0].getNb(), 1u);
}

Test(order, pizza_type_and_size_match) {
  auto orders = plazza::Parsing::parsingOrder("americana XL x2");
  cr_assert_eq(orders[0].getPizzaType(), Americana);
  cr_assert_eq(orders[0].getPizzaSize(), PizzaSize::XL);
  cr_assert_eq(orders[0].getNb(), 2u);
}

Test(pizza_factory, create_margarita_type_string) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert_str_eq(p->getType().c_str(), "Margarita");
}

Test(pizza_factory, create_americana_type_string) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::M);
  cr_assert_str_eq(p->getType().c_str(), "Americana");
}

Test(pizza_factory, create_regina_type_string) {
  auto p = plazza::PizzaFactory::createPizza(Regina, PizzaSize::M);
  cr_assert_str_eq(p->getType().c_str(), "Regina");
}

Test(pizza_factory, create_fantasia_type_string) {
  auto p = plazza::PizzaFactory::createPizza(Fantasia, PizzaSize::M);
  cr_assert_str_eq(p->getType().c_str(), "Fantasia");
}

Test(pizza_factory, margarita_baking_time_1s) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert_eq(p->getBakingTime(), 1u);
}

Test(pizza_factory, americana_baking_time_2s) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::M);
  cr_assert_eq(p->getBakingTime(), 2u);
}

Test(pizza_factory, regina_baking_time_2s) {
  auto p = plazza::PizzaFactory::createPizza(Regina, PizzaSize::M);
  cr_assert_eq(p->getBakingTime(), 2u);
}

Test(pizza_factory, fantasia_baking_time_4s) {
  auto p = plazza::PizzaFactory::createPizza(Fantasia, PizzaSize::M);
  cr_assert_eq(p->getBakingTime(), 4u);
}

Test(pizza_factory, margarita_has_dough) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert(p->getIngredients() & DOUGH);
}

Test(pizza_factory, margarita_has_tomato) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert(p->getIngredients() & TOMATO);
}

Test(pizza_factory, margarita_has_gruyere) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert(p->getIngredients() & GRUYERE);
}

Test(pizza_factory, americana_has_steak) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::M);
  cr_assert(p->getIngredients() & STEAK);
}

Test(pizza_factory, regina_has_ham) {
  auto p = plazza::PizzaFactory::createPizza(Regina, PizzaSize::M);
  cr_assert(p->getIngredients() & HAM);
}

Test(pizza_factory, regina_has_mushrooms) {
  auto p = plazza::PizzaFactory::createPizza(Regina, PizzaSize::M);
  cr_assert(p->getIngredients() & MUSHROOMS);
}

Test(pizza_factory, fantasia_has_eggplant) {
  auto p = plazza::PizzaFactory::createPizza(Fantasia, PizzaSize::M);
  cr_assert(p->getIngredients() & EGGPLANT);
}

Test(pizza_factory, fantasia_has_goatcheese) {
  auto p = plazza::PizzaFactory::createPizza(Fantasia, PizzaSize::M);
  cr_assert(p->getIngredients() & GOATCHEESE);
}

Test(pizza_factory, fantasia_has_chieflove) {
  auto p = plazza::PizzaFactory::createPizza(Fantasia, PizzaSize::M);
  cr_assert(p->getIngredients() & CHIEFLOVE);
}

Test(pizza_factory, size_s_stored) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::S);
  cr_assert_eq(p->getSize(), PizzaSize::S);
}

Test(pizza_factory, size_xxl_stored) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::XXL);
  cr_assert_eq(p->getSize(), PizzaSize::XXL);
}

Test(pizza_factory, size_string_m) {
  auto p = plazza::PizzaFactory::createPizza(Margarita, PizzaSize::M);
  cr_assert_str_eq(p->getSizeString().c_str(), "M");
}

Test(pizza_factory, size_string_xl) {
  auto p = plazza::PizzaFactory::createPizza(Regina, PizzaSize::XL);
  cr_assert_str_eq(p->getSizeString().c_str(), "XL");
}

Test(pizza_factory, ingredients_list_not_empty) {
  auto p = plazza::PizzaFactory::createPizza(Americana, PizzaSize::M);
  cr_assert_gt(p->getIngredientsList().size(), 0u);
}

Test(threadpool, size_returns_n) {
  plazza::ThreadPool pool(3);
  cr_assert_eq(pool.size(), 3u);
}

Test(threadpool, size_one) {
  plazza::ThreadPool pool(1);
  cr_assert_eq(pool.size(), 1u);
}

Test(threadpool, size_ten) {
  plazza::ThreadPool pool(10);
  cr_assert_eq(pool.size(), 10u);
}

Test(threadpool, initial_busy_zero) {
  plazza::ThreadPool pool(3);
  cr_assert_eq(pool.busy(), 0u);
}

Test(threadpool, initial_pending_zero) {
  plazza::ThreadPool pool(3);
  cr_assert_eq(pool.pending(), 0u);
}

Test(threadpool, enqueue_task_runs) {
  plazza::ThreadPool pool(2);
  std::atomic<bool> ran{false};
  pool.enqueue([&ran] { ran = true; });
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  cr_assert(ran.load());
}

Test(threadpool, all_tasks_run) {
  plazza::ThreadPool pool(4);
  std::atomic<int> count{0};
  for (int i = 0; i < 20; i++)
    pool.enqueue([&count] { count++; });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  cr_assert_eq(count.load(), 20);
}

Test(threadpool, concurrent_tasks_no_race) {
  plazza::ThreadPool pool(5);
  std::atomic<int> counter{0};
  for (int i = 0; i < 50; i++)
    pool.enqueue([&counter] { counter.fetch_add(1); });
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  cr_assert_eq(counter.load(), 50);
}

Test(threadpool, pending_drops_to_zero) {
  plazza::ThreadPool pool(2);
  std::atomic<bool> block{true};
  pool.enqueue([&block] {
    while (block.load())
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
  });
  pool.enqueue([&block] {
    while (block.load())
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  block = false;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  cr_assert_eq(pool.pending(), 0u);
}

Test(threadpool, tasks_run_after_many_enqueues) {
  plazza::ThreadPool pool(3);
  std::atomic<int> n{0};
  for (int i = 0; i < 100; i++)
    pool.enqueue([&n] { n++; });
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  cr_assert_eq(n.load(), 100);
}

Test(threadpool, size_unchanged_after_enqueue) {
  plazza::ThreadPool pool(4);
  pool.enqueue([] {});
  pool.enqueue([] {});
  cr_assert_eq(pool.size(), 4u);
}
