/* Transrangers: an efficient, composable design pattern for range processing.
 *
 * Copyright 2021 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See https://github.com/joaquintides/transrangers for project home page.
 */

#ifndef JOAQUINTIDES_TRANSRANGERS_HPP
#define JOAQUINTIDES_TRANSRANGERS_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <iterator>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

#if defined(__clang__)
#define TRANSRANGERS_HOT __attribute__((flatten))
#define TRANSRANGERS_HOT_MUTABLE __attribute__((flatten)) mutable
#elif defined(__GNUC__)
#define TRANSRANGERS_HOT __attribute__((flatten))
#define TRANSRANGERS_HOT_MUTABLE mutable __attribute__((flatten))
#else
#define TRANSRANGERS_HOT [[msvc::forceinline]]
#define TRANSRANGERS_HOT_MUTABLE mutable [[msvc::forceinline]]
#endif

namespace transrangers {

/**
 * @brief ranger_class
 *
 * The `ranger_class` template is a helper class used in the Transrangers
 * library. It is used to define a ranger object that represents a range
 * processing operation. The `ranger_class` template takes two template
 * parameters: `Cursor` and `F`. `Cursor` represents the iterator type of the
 * range, and `F` represents the type of the function object that will be
 * applied to each element of the range.
 *
 * @tparam Cursor
 * @tparam F
 */
template <typename Cursor, typename F> struct ranger_class : F {
  using cursor = Cursor;
};

/**
 * @brief ranger(F t)
 *
 * The `ranger(F f)` function is a helper function that creates a `ranger_class`
 * object. It takes a function object `f` as input and returns a `ranger_class`
 * object with the function object `f` as its base class. The `ranger_class`
 * represents a range processing operation that can be composed with other
 * ranger operations.
 *
 * @tparam Cursor
 * @tparam F
 * @param f
 * @return auto
 */
template <typename Cursor, typename F> auto ranger(F f) {
  return ranger_class<Cursor, F>{f};
}

// all, all_copy

/**
 * @brief all
 *
 * The `all` function takes a range `rng` as input and returns a ranger object.
 * The ranger object represents a range processing operation that can be
 * composed with other ranger operations.
 *
 * @tparam Range
 * @param rng
 * @return auto
 */
template <typename Range> auto all(Range &&rng) {
  using std::begin;
  using std::end;
  using cursor = decltype(begin(rng));

  return ranger<cursor>([first = begin(rng), last = end(rng)](auto dst)
                            TRANSRANGERS_HOT_MUTABLE {
                              auto it = first;
                              while (it != last)
                                if (!dst(it++)) {
                                  first = it;
                                  return false;
                                }
                              return true;
                            });
}

/**
 * @brief all_copy
 *
 * The `all_copy` struct is a helper struct used in the `all` function. It is
 * used to handle the case when the input range passed to `all` is an rvalue
 * reference. It stores the input range and creates a ranger object using the
 * `all` function.
 *
 * @tparam Range
 */
template <typename Range> struct all_copy {
  using ranger = decltype(all(std::declval<Range &>()));
  using cursor = typename ranger::cursor;

  template <typename F> auto operator()(const F &p) { return rgr(p); }

  Range rng;
  ranger rgr = all(rng);
};

/**
 * @brief all(Range &&rng)
 *
 * The `all(Range &&rng)` function takes a range `rng` as input and returns a
 * ranger object. The ranger object represents a range processing operation that
 * can be composed with other ranger operations. The `all` function iterates
 * over the elements in the input range and applies a given function to each
 * element. If the function returns `false` for any element, the iteration stops
 * and `false` is returned. If the function returns `true` for all elements, the
 * iteration continues until all elements have been processed and `true` is
 * returned.
 *
 * @tparam Range
 * @param rng
 * @return std::enable_if<std::is_rvalue_reference<Range &&>::value,
 * all_copy<Range>>::type
 */
template <typename Range>
typename std::enable_if<std::is_rvalue_reference<Range &&>::value,
                        all_copy<Range>>::type
all(Range &&rng) {
  return all_copy<Range>{std::move(rng)};
}

// filter

/**
 * @brief pred_box
 *
 * The `pred_box` function is a helper function that takes a predicate function
 * `pred` as input and returns a lambda function. The lambda function captures
 * the predicate function `pred` by value and takes a variable number of
 * arguments `x`. It then forwards these arguments to the predicate function
 * `pred` using perfect forwarding and returns the result. The lambda function
 * has a return type of `int`.
 *
 * @tparam Pred
 * @param pred
 * @return auto
 */
template <typename Pred> auto pred_box(Pred pred) {
  return
      [=](auto &&...x) -> int { return pred(std::forward<decltype(x)>(x)...); };
}

/**
 * @brief filter
 *
 * @tparam Pred
 * @tparam Ranger
 * @param pred_
 * @param rgr
 * @return auto
 */
template <typename Pred, typename Ranger> auto filter(Pred pred_, Ranger rgr) {
  using cursor = typename Ranger::cursor;

  return ranger<cursor>(
      [=, pred = pred_box(pred_)](auto dst) TRANSRANGERS_HOT_MUTABLE {
        return rgr([&](const auto &p)
                       TRANSRANGERS_HOT { return pred(*p) ? dst(p) : true; });
      });
}

/**
 * @brief deref_fun
 *
 * @tparam Cursor
 * @tparam F
 * @tparam typename
 */
template <typename Cursor, typename F, typename = void> struct deref_fun {
  decltype(auto) operator*() const { return (*pf)(*p); }

  Cursor p;
  F *pf;
};

/**
 * @brief
 *
 * The below code is defining a specialization of the `deref_fun` struct
 * template. This specialization is for cases where the type `F` is trivially
 * default constructible and empty.
 *
 * @tparam Cursor
 * @tparam F
 */
template <typename Cursor, typename F>
struct deref_fun<
    Cursor, F,
    typename std::enable_if<std::is_trivially_default_constructible<F>::value &&
                            std::is_empty<F>::value>::type> {
  deref_fun(Cursor p = {}, F * = nullptr) : p{p} {}

  decltype(auto) operator*() const { return F()(*p); }

  Cursor p;
};

/**
 * @brief transform
 *
 * The below code is defining a function template called `transform`. This
 * function takes two parameters: `f`, which is a callable object, and `rgr`,
 * which is an object of a type that satisfies the `Ranger` concept.
 *
 * @tparam F
 * @tparam Ranger
 * @param f
 * @param rgr
 * @return auto
 */
template <typename F, typename Ranger> auto transform(F f, Ranger rgr) {
  using cursor = deref_fun<typename Ranger::cursor, F>;

  return ranger<cursor>([=](auto dst) TRANSRANGERS_HOT_MUTABLE {
    return rgr([&](const auto &p) TRANSRANGERS_HOT {
      return dst(cursor{p, &f});
    });
  });
}

/**
 * @brief take
 *
 * The below code is defining a function template called `take` that takes an
 * integer `n` and a `Ranger` object as arguments. The `Ranger` type is expected
 * to have a nested type called `cursor`.
 *
 * @tparam Ranger
 * @param n
 * @param rgr
 * @return auto
 */
template <typename Ranger> auto take(int n, Ranger rgr) {
  using cursor = typename Ranger::cursor;

  return ranger<cursor>([=](auto dst) TRANSRANGERS_HOT_MUTABLE {
    if (n)
      return rgr([&](const auto &p) TRANSRANGERS_HOT {
               --n;
               return dst(p) && (n != 0);
             }) ||
             (n == 0);
    else
      return true;
  });
}

/**
 * @brief
 *
 * @tparam Ranger
 * @param rgr
 * @return auto
 */
template <typename Ranger> auto concat(Ranger rgr) { return rgr; }

/**
 * @brief
 *
 * @tparam Ranger
 * @tparam Rangers
 * @param rgr
 * @param rgrs
 * @return auto
 */
template <typename Ranger, typename... Rangers>
auto concat(Ranger rgr, Rangers... rgrs) {
  using cursor = typename Ranger::cursor;

  return ranger<cursor>([=, cont = false, next = concat(rgrs...)](auto dst)
                            TRANSRANGERS_HOT_MUTABLE {
                              if (!cont) {
                                if (!(cont = rgr(dst)))
                                  return false;
                              }
                              return next(dst);
                            });
}
// The above code is implementing a function template called `concat` that
// concatenates multiple ranges together.

// unique
template <typename Ranger> auto unique(Ranger rgr) {
  using cursor = typename Ranger::cursor;

  return ranger<cursor>(
      [=, start = true, p = cursor{}](auto dst) TRANSRANGERS_HOT_MUTABLE {
        if (start) {
          start = false;
          if (rgr([&](const auto &q) TRANSRANGERS_HOT {
                p = q;
                return false;
              }))
            return true;
          if (!dst(p))
            return false;
        }
        return rgr([&, prev = p](const auto &q) TRANSRANGERS_HOT_MUTABLE {
          if ((*prev == *q) || dst(q)) {
            prev = q;
            return true;
          } else {
            p = q;
            return false;
          }
        });
      });
}

// join
struct identity_adaption {
  template <typename T> static decltype(auto) adapt(T &&srgr) {
    return std::forward<decltype(srgr)>(srgr);
  }
};

template <typename Ranger, typename Adaption = identity_adaption>
auto join(Ranger rgr) {
  using cursor = typename Ranger::cursor;
  using subranger =
      std::remove_cv_t<std::remove_reference_t<decltype(Adaption::adapt(
          *std::declval<const cursor &>()))>>;
  using subranger_cursor = typename subranger::cursor;

  return ranger<subranger_cursor>([=, osrgr = std::optional<subranger>{}](
                                      auto dst) TRANSRANGERS_HOT_MUTABLE {
    if (osrgr) {
      if (!(*osrgr)(dst))
        return false;
    }
    return rgr([&](const auto &p) TRANSRANGERS_HOT {
      auto srgr = Adaption::adapt(*p);
      if (!srgr(dst)) {
        osrgr.emplace(std::move(srgr));
        return false;
      } else
        return true;
    });
  });
}

struct all_adaption {
  template <typename T> static auto adapt(T &&srgn) {
    return all(std::forward<decltype(srgn)>(srgn));
  }
};

template <typename Ranger> auto ranger_join(Ranger rgr) {
  return join<Ranger, all_adaption>(std::move(rgr));
}
// The above code defines a function template called `join` that takes a single
// argument `rgr`. The function template has a default template parameter
// `Adaption` which is set to `identity_adaption`.

// zip
/**
 * @brief zip_cursor
 *
 * The above code defines a template struct called `zip_cursor`. This struct is
 * used to iterate over multiple ranges simultaneously, similar to the `zip`
 * function in Python.
 *
 * @tparam Rangers
 */
template <typename... Rangers> struct zip_cursor {
  auto operator*() const {
    return std::apply(
        [](const auto &...ps) { return std::tuple<decltype(*ps)...>{*ps...}; },
        ps);
  }

  std::tuple<typename Rangers::cursor...> ps;
};

// template <typename Ranger, typename... Rangers>
// auto zip(Ranger rgr, Rangers... rgrs) {
//   using cursor = zip_cursor<Ranger, Rangers...>;
//
//   return ranger<cursor>([=, zp = cursor{}](auto dst) TRANSRANGERS_HOT_MUTABLE
//   {
//     bool finished = false;
//     return rgr([&finished, &zp, &dst,
//                 &rgrs...](const auto &p) TRANSRANGERS_HOT {
//              std::get<0>(zp.ps) = p;
//              if ([&zp, &rgrs... ]<std::size_t... I>(std::index_sequence<I...>
// #ifdef _MSC_VER
//                                                     ,
//                                                     auto &...rgrs
// #endif
//                                                     ) TRANSRANGERS_HOT {
//                    return (rgrs([&zp](const auto &p) TRANSRANGERS_HOT {
//                              std::get<I + 1>(zp.ps) = p;
//                              return false;
//                            }) ||
//                            ...);
//                  }(std::index_sequence_for<Rangers...>{}
// #ifdef _MSC_VER
//                    ,
//                    rgrs...
// #endif
//                    )) {
//                finished = true;
//                return false;
//              }
//
//              return dst(zp);
//            }) ||
//            finished;
//   });
// }

// template <typename Ranger1, typename Ranger2> struct zip2_cursor {
//   auto operator*() const {
//     return std::apply(
//         [](const auto &p1, const auto &p2) {
//           return std::tuple<decltype(p1), decltype(p2)>{*p1, *p2};
//         },
//         ps);
//   }
//
//   std::tuple<typename Ranger1::cursor, typename Ranger2::cursor> ps;
// };

/**
 * @brief zip2
 *
 * The below code is defining a function template called `zip2` that takes two
 * ranges (`Ranger1` and `Ranger2`) as input. It returns a new range that
 * represents the zipped version of the two input ranges.
 *
 * @tparam Ranger1
 * @tparam Ranger2
 * @param rgr1
 * @param rgr2
 * @return auto
 */
template <typename Ranger1, typename Ranger2>
auto zip2(Ranger1 rgr1, Ranger2 rgr2) {
  using cursor = zip_cursor<Ranger1, Ranger2>;

  return ranger<cursor>([=, zp = cursor{}](auto dst) TRANSRANGERS_HOT_MUTABLE {
    bool finished = false;
    return rgr1([&](const auto &p) TRANSRANGERS_HOT {
             std::get<0>(zp.ps) = p;
             if (rgr2([&](const auto &p) TRANSRANGERS_HOT {
                   std::get<1>(zp.ps) = p;
                   return false;
                 })) {
               finished = true;
               return false;
             }
             return dst(zp);
           }) ||
           finished;
  });
}

// accumulate

/**
 * @brief accumulate
 *
 * The above code is defining a template function called `accumulate` that takes
 * two parameters: `rgr` and `init`.
 *
 * @tparam Ranger
 * @tparam T
 * @param rgr
 * @param init
 * @return T
 */
template <typename Ranger, typename T> T accumulate(Ranger rgr, T init) {
  rgr([&](const auto &p) TRANSRANGERS_HOT {
    init = std::move(init) + *p;
    return true;
  });
  return init;
}

} // namespace transrangers

#undef TRANSRANGERS_HOT_MUTABLE
#undef TRANSRANGERS_HOT
#endif
