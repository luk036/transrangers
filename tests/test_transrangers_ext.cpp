#include <doctest/doctest.h>

#include <range/v3/view/all.hpp>
#include <transranger_view.hpp>
#include <transrangers_ext.hpp>
#include <vector>

TEST_CASE("Test transrangers (skip_first)") {
  using namespace transrangers;

  const auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, skip_first(S));
  auto total = accumulate(rng, 6); // 6 + 3
  CHECK_EQ(total, 9);
}

TEST_CASE("Test transrangers (skip_last)") {
  using namespace transrangers;

  const auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, skip_last(S));
  auto total = accumulate(rng, 6); // 6 + 1 + 3
  CHECK_EQ(total, 10);
}

TEST_CASE("Test transrangers (skip_both)") {
  using namespace transrangers;

  const auto S = std::vector<int>{1, 2, 3, 4, 5};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng = filter(is_odd, skip_both(S));
  auto total = accumulate(rng, 6); // 6 + 3
  CHECK_EQ(total, 9);
}

TEST_CASE("Test transrangers (enumerate)") {
  using namespace transrangers;

  const auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto sum = [](const auto &p) { return std::get<0>(p) + std::get<1>(p); };
  auto rng = transform(sum, enumerate(filter(is_odd, all(S))));
  auto total = accumulate(rng, 0); // 0 + 1 + 1 + 3
  CHECK_EQ(total, 5);
}

TEST_CASE("Test transrangers (enumerate + input_view)") {
  using namespace transrangers;

  auto S = std::vector<int>{1, 2, 3, 4};
  auto is_odd = [](int a) { return a % 2 == 1; };
  auto rng1 = enumerate(filter(is_odd, all(S)));
  auto total = 0;
  for (auto [i, e] : input_view(rng1)) {
    total += i + e;
  }
  // CHECK_EQ(total, 5);
}

TEST_CASE("Test transrangers (partial sum)") {
  using namespace transrangers;

  auto S = std::vector<int>{1, 2, 3, 4};
  auto init = *S.begin();
  auto total = partial_sum(skip_first(S), init);
  CHECK_EQ(total, 10);
  CHECK_EQ(S[3], 10);
}
