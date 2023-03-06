/* Transrangers performance benchmark.
 *
 * Copyright 2021 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See https://github.com/joaquintides/transrangers for project home page.
 */

#include <algorithm>
#include <functional>
#include <iostream>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <numeric>
#include <transrangers.hpp>
#include <vector>

int main() {
  volatile int ret;

  auto is_even = [](int x) { return x % 2 == 0; };
  auto x3 = [](int x) { return 3 * x; };

  auto rng1 = [] {
    std::vector<int> rng1(1000000);
    std::iota(rng1.begin(), rng1.end(), 0);
    return rng1;
  }();

  auto test1_handwritten = [&] {
    int res = 0;
    for (auto x : rng1) {
      if (is_even(x))
        res += x3(x);
    }
    ret = res;
  };

  auto test1_transrangers = [&] {
    using namespace transrangers;

    ret = accumulate(transform(x3, filter(is_even, all(rng1))), 0);
  };

  auto rng2 = rng1;
  int n = rng2.size() + rng2.size() / 2;

  auto test2_handwritten = [&] {
    int res = 0;
    int m = n;
    auto f = [&] {
      for (auto first = std::begin(rng2), last = std::end(rng2);
           m && first != last; --m, ++first) {
        auto &&x = *first;
        if (is_even(x))
          res += x3(x);
      }
    };
    f();
    f();
    ret = res;
  };

  auto test2_transrangers = [&] {
    using namespace transrangers;

    ret = accumulate(
        transform(x3, filter(is_even, take(n, concat(all(rng2), all(rng2))))),
        0);
  };

  auto rng3 = [] {
    std::vector<int> rng3;
    for (int i = 0; i < 100000 / 4; ++i) {
      rng3.push_back(i);
      rng3.push_back(i);
      rng3.push_back(i);
      rng3.push_back(i);
    }
    return rng3;
  }();

  auto test3_handwritten = [&] {
    int res = 0;
    int x = rng3[0] + 1;
    for (int y : rng3) {
      if (y != x) {
        x = y;
        if (is_even(x))
          res += x;
      }
    }
    ret = res;
  };

  auto test3_transrangers = [&] {
    using namespace transrangers;

    ret = accumulate(filter(is_even, unique(all(rng3))), 0);
  };

  auto rng4 = [] {
    std::vector<int> srng;
    for (int i = 0; i < 100000 / 4; ++i) {
      srng.push_back(i);
      srng.push_back(i);
      srng.push_back(i);
      srng.push_back(i);
    }
    std::vector<std::vector<int>> rng4(10, srng);
    return rng4;
  }();

  auto test4_handwritten = [&] {
    int res = 0;
    int x = rng4[0][0] + 1;
    for (auto &&srng : rng4) {
      for (int y : srng) {
        if (y != x) {
          x = y;
          if (is_even(x))
            res += x3(x);
        }
      }
    }
    ret = res;
  };

  auto test4_transrangers = [&] {
    using namespace transrangers;

    ret = accumulate(
        transform(x3, filter(is_even, unique(ranger_join(all(rng4))))), 0);
  };

  auto rng5 = rng4;

  auto test5_handwritten = [&] {
    int res = 0;
    for (auto &&srng : rng5) {
      int x = srng[0] + 1;
      for (int y : srng) {
        if (y != x) {
          x = y;
          if (is_even(x))
            res += x3(x);
        }
      }
    }
    ret = res;
  };

  auto test5_transrangers = [&] {
    using namespace transrangers;

    auto unique_adaptor = [](auto &&srng) {
      return unique(all(std::forward<decltype(srng)>(srng)));
    };
    ret = accumulate(
        transform(x3,
                  filter(is_even, join(transform(unique_adaptor, all(rng5))))),
        0);
  };

  auto divisible_by_3 = [](int x) { return x % 3 == 0; };
  auto sum = [](const auto &p) { return std::get<0>(p) + std::get<1>(p); };
  auto rng6 = rng1;

  auto test6_handwritten = [&] {
    int res = 0;
    for (auto x : rng6) {
      auto y = x + x3(x);
      if (divisible_by_3(y))
        res += y;
    }
    ret = res;
  };

  auto test6_transrangers = [&] {
    using namespace transrangers;

    ret = accumulate(
        filter(divisible_by_3,
               transform(sum, zip(all(rng6), transform(x3, all(rng6))))),
        0);
  };
}
