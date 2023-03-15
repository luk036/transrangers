#include <doctest/doctest.h>

#include <transranger_view.hpp>
#include <transrangers.hpp>

#include <map>
#include <unordered_map>
#include <vector>

TEST_CASE("Test transrangers (all)") {
  using namespace transrangers;

  auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, all(S));
  auto total = accumulate(rng, 0); // 0 + 1 + 3
  CHECK_EQ(total, 4);
}

TEST_CASE("Test transrangers (zip2)") {
  using namespace transrangers;

  auto I = std::vector<int>{0, 1, 2, 3};
  auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto sum = [](const auto &p) { return std::get<0>(p) + std::get<1>(p); };
  auto rng = transform(sum, zip2(all(I), filter(is_odd, all(S))));
  auto total = accumulate(rng, 0); // 0 + 1 + 1 + 3
  CHECK_EQ(total, 5);
}

TEST_CASE("Test transrangers (zip2 + input_view)") {
  using namespace transrangers;

  auto I = std::vector<int>{0, 1, 2, 3};
  auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng1 = zip2(all(I), filter(is_odd, all(S)));
  auto total = 0;
  for (auto [i, e] : input_view(rng1)) {
    total += i + e;
  }
  CHECK_EQ(total, 5);
}

TEST_CASE("Test map") {
  using Edge = std::pair<int, int>;

  auto S = std::unordered_map<int, int>{Edge{0, 1}, Edge{2, 3}, Edge{3, 4},
                                        Edge{4, 2}};
  auto count = 0;
  for ([[maybe_unused]] auto e : S) {
    ++count;
  }
  CHECK_EQ(count, 4);
}

TEST_CASE("Test transrangers (map)") {
  using namespace transrangers;
  using Edge = std::pair<int, int>;

  auto S = std::unordered_map<int, int>{Edge{0, 1}, Edge{2, 3}, Edge{3, 4},
                                        Edge{4, 2}};
  auto rng = all(S);
  auto count = 0;
  rng([&count](const auto &) {
    ++count;
    return true;
  });

  CHECK_EQ(count, 4);
}
