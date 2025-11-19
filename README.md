# curry-cpp

C++17 header-only library for function currying.

## Features

- most of the callable types supported (anything that can be put into `std::function`)
- Support any number of arguments in multiple number of calls (see example below)
- no template parameter required

## Usage

Just include the header:

```cpp
#include "curry.hpp"
```

Then you can use the `curry::curried` function to create a curried version of
your function.

## Example

```cpp
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
```

See the full example in [test.cpp](./test.cpp).
