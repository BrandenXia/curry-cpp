#include <iostream>

#include "curry.hpp"

using curry::curried;

int main() {
  auto curried_f = curried([](int a, float b, double c) { return a + b + c; });
  auto res = curried_f(1, 2.f, 3.0);
  std::cout << "res1: " << res << "\n";

  auto a = curried_f(2, 3.f);
  res = a(4.0);
  std::cout << "res2: " << res << "\n";

  auto b = curried_f(3);
  res = b(4.f, 5.0);
  std::cout << "res3: " << res << "\n";

  auto c = b(6.f);
  res = c(7.0);
  std::cout << "res4: " << res << "\n";
}
