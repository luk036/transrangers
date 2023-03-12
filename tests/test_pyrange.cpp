#include <doctest/doctest.h>

#include <pyrange/enumerate.hpp>
#include <pyrange/range.hpp>
#include <range/v3/view/all.hpp>
#include <transranger_view.hpp>
#include <transrangers.hpp>
#include <vector>

TEST_CASE("Test transrangers (all)") {
  using namespace transrangers;

  auto S = py::range(1, 5);
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, all(S));
  auto total = accumulate(rng, 0); // 0 + 1 + 3
  CHECK_EQ(total, 4);
}

TEST_CASE("Test transrangers (skip)") {
  using namespace transrangers;

  auto S = py::range(1, 5);
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, skip(S));
  auto total = accumulate(rng, 6); // 6 + 3
  CHECK_EQ(total, 9);
}

TEST_CASE("Test transrangers (zip2)") {
  using namespace transrangers;

  auto I = py::range<int>(4);
  auto S = py::range(1, 5);
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto sum = [](const auto &p) { return std::get<0>(p) + std::get<1>(p); };
  auto rng = transform(sum, zip2(all(I), filter(is_odd, all(S))));
  auto total = accumulate(rng, 0); // 0 + 1 + 1 + 3
  CHECK_EQ(total, 5);
}

TEST_CASE("Test transrangers (zip2 + input_view)") {
  using namespace transrangers;

  auto I = py::range<int>(4);
  auto S = py::range(1, 5);
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng1 = zip2(all(I), filter(is_odd, all(S)));
  auto total = 0;
  for (auto [i, e] : input_view(rng1)) {
    total += i + e;
  }
  CHECK_EQ(total, 5);
}

// TEST_CASE("Test transrangers (enumerate)") {
//   using namespace transrangers;
//
//   auto S = std::vector<int>{1, 2, 3, 4};
//   auto is_odd = [](int a) { return a % 2 == 1; };
//   auto sum = [](const auto &p) { return std::get<0>(p) + std::get<1>(p); };
//   auto rng = transform(sum, enumerate(filter(is_odd, all(S))));
//   auto total = accumulate(rng, 0); // 0 + 1 + 1 + 3
//   CHECK_EQ(total, 5);
// }

TEST_CASE("Test transrangers (enumerate + input_view)") {
  using namespace transrangers;

  auto S = std::vector<int>{1, 2, 3, 4};
  // auto is_odd = [](int a) { return a % 2 == 1; };
  // auto rng1 = filter(is_odd, all(S));
  auto total = 0;
  for (auto [i, e] : py::enumerate(S)) {
    total += i + e;
  }
  // CHECK_EQ(total, 5);
}
