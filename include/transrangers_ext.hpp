/* Transrangers: an efficient, composable design pattern for range processing.
 *
 * Copyright 2021 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See https://github.com/joaquintides/transrangers for project home page.
 */

#ifndef JOAQUINTIDES_TRANSRANGERS_EXT_HPP
#define JOAQUINTIDES_TRANSRANGERS_EXT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include "transrangers.hpp"

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

// skip_first, skip_first_copy (assume the next item is available)

/**
 * @brief skip_first (assume the next item is available)
 *
 * The `skip_first` function is a part of the Transrangers library, which
 * provides an efficient and composable design pattern for range processing.
 *
 * @tparam Range
 * @param rng
 * @return auto
 */
template <typename Range> auto skip_first(Range &&rng) {
  using std::begin;
  using std::end;
  using cursor = decltype(begin(rng));

  // Sorry, I don't check if the second item is available.
  return ranger<cursor>([first = ++begin(rng), last = end(rng)](auto dst)
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
 * @brief
 *
 * @tparam Range
 */
template <typename Range> struct skip_first_copy {
  using ranger = decltype(skip_first(std::declval<Range &>()));
  using cursor = typename ranger::cursor;

  template <typename F> auto operator()(const F &p) { return rgr(p); }

  Range rng;
  ranger rgr = skip_first(rng);
};

template <typename Range>
typename std::enable_if<std::is_rvalue_reference<Range &&>::value,
                        skip_first_copy<Range>>::type
skip_first(Range &&rng) {
  return skip_first_copy<Range>{std::move(rng)};
}

// skip_last, skip_last_copy (assume the previous item is available)
template <typename Range> auto skip_last(Range &&rng) {
  using std::begin;
  using std::end;
  using cursor = decltype(begin(rng));

  // Sorry, I don't check if the second last item is available.
  return ranger<cursor>([first = begin(rng), last = --end(rng)](auto dst)
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

template <typename Range> struct skip_last_copy {
  using ranger = decltype(skip_last(std::declval<Range &>()));
  using cursor = typename ranger::cursor;

  template <typename F> auto operator()(const F &p) { return rgr(p); }

  Range rng;
  ranger rgr = skip_last(rng);
};

template <typename Range>
typename std::enable_if<std::is_rvalue_reference<Range &&>::value,
                        skip_last_copy<Range>>::type
skip_last(Range &&rng) {
  return skip_last_copy<Range>{std::move(rng)};
}

// skip_first, skip_first_copy (assume the next item is available)
template <typename Range> auto skip_both(Range &&rng) {
  using std::begin;
  using std::end;
  using cursor = decltype(begin(rng));

  // Sorry, I don't check if the second item is available.
  return ranger<cursor>([first = ++begin(rng), last = --end(rng)](auto dst)
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

template <typename Range> struct skip_both_copy {
  using ranger = decltype(skip_both(std::declval<Range &>()));
  using cursor = typename ranger::cursor;

  template <typename F> auto operator()(const F &p) { return rgr(p); }

  Range rng;
  ranger rgr = skip_both(rng);
};

template <typename Range>
typename std::enable_if<std::is_rvalue_reference<Range &&>::value,
                        skip_both_copy<Range>>::type
skip_both(Range &&rng) {
  return skip_both_copy<Range>{std::move(rng)};
}

/**
 * @brief Enumerate
 *
 * The `enumerate` function is a part of the Transrangers library. It takes a
 * range `rgr` as input and returns a new range where each element is paired
 * with its corresponding index.
 *
 * @tparam Ranger
 * @param rgr
 * @return auto
 */
template <typename Ranger> auto enumerate(Ranger rgr) {
  return transform(
      [index = std::size_t(0)](auto &&value) TRANSRANGERS_HOT_MUTABLE {
        auto old = index;
        index += 1;
        return std::make_pair(old, std::move(value));
      },
      rgr);
}

/**
 * @brief partial sum (cummutative sum)
 *
 * The `partial_sum` function is a part of the Transrangers library. It takes a
 * range `rgr` and an initial value `init` as input and returns the cumulative
 * sum of the elements in the range.
 *
 * @tparam Ranger
 * @tparam T
 * @param rgr
 * @param init
 * @return T
 */
template <typename Ranger, typename T> T partial_sum(Ranger rgr, T init) {
  rgr([&init](const auto &p) TRANSRANGERS_HOT {
    init = std::move(init) + *p;
    *p = init;
    return true;
  });
  return init;
}

/**
 * @brief __lambda_255_33
 *
 * The code snippet defines a lambda class named `__lambda_255_33` that is used
 * as a helper for the `zip` function in the Transrangers library.
 *
 * @tparam I
 * @tparam Ranger
 * @tparam Rangers
 */
template <std::size_t I, typename Ranger, typename... Rangers>
class __lambda_255_33 {
  using cursor = zip_cursor<Ranger, Rangers...>;

public:
  template <class type_parameter_4_0>
  TRANSRANGERS_HOT inline auto operator()(const type_parameter_4_0 &p) const {
    std::get<I + 1>(zp.ps) = p;
    return false;
  }

private:
  cursor &zp;

public:
  __lambda_255_33(cursor &_zp) : zp{_zp} {}
};

template <typename Ranger, typename... Rangers> class __lambda_249_18 {
  using cursor = zip_cursor<Ranger, Rangers...>;

public:
  template <std::size_t... I>
  TRANSRANGERS_HOT inline auto operator()(std::index_sequence<I...>) const {
    return (std::get<I>(rgrs)(__lambda_255_33<I, Ranger, Rangers...>{zp}) ||
            ...);
  }

private:
  cursor &zp;
  std::tuple<Rangers...> &rgrs;

public:
  __lambda_249_18(cursor &_zp, std::tuple<Rangers...> &_rgrs)
      : zp{_zp}, rgrs{_rgrs} {}
};

template <typename type_parameter_1_0, typename Ranger, typename... Rangers>
class __lambda_246_16 {
  using cursor = zip_cursor<Ranger, Rangers...>;

public:
  template <class type_parameter_2_0>
  TRANSRANGERS_HOT inline auto operator()(const type_parameter_2_0 &p) const {
    std::get<0>(zp.ps) = p;
    if (__lambda_249_18{zp, rgrs}(std::index_sequence_for<Rangers...>{})) {
      finished = true;
      return false;
    }

    return dst(zp);
  }

private:
  bool &finished;
  type_parameter_1_0 &dst;
  cursor &zp;
  std::tuple<Rangers...> &rgrs;

public:
  __lambda_246_16(bool &_finished, type_parameter_1_0 &_dst, cursor &_zp,
                  std::tuple<Rangers...> &_rgrs)
      : finished{_finished}, dst{_dst}, zp{_zp}, rgrs{_rgrs} {}
};

template <typename Ranger, typename... Rangers> class __lambda_244_25 {
  using cursor = zip_cursor<Ranger, Rangers...>;

public:
  template <class type_parameter_1_0>
  TRANSRANGERS_HOT inline auto operator()(type_parameter_1_0 dst) {
    bool finished = false;
    return rgr(__lambda_246_16{finished, dst, zp, rgrs}) || finished;
  }

private:
  cursor zp;
  Ranger rgr;
  std::tuple<Rangers...> rgrs;

public:
  __lambda_244_25(cursor _zp, const Ranger &_rgr, const Rangers &..._rgrs)
      : zp{_zp}, rgr{_rgr}, rgrs{_rgrs...} {}
};

/**
 * @brief zip
 *
 * The `zip` function is a part of the Transrangers library. It takes a range
 * `rgr` and one or more additional ranges `rgrs` as input and returns a new
 * range where each element is a tuple containing the corresponding elements
 * from all the input ranges.
 *
 * @tparam Ranger
 * @tparam Rangers
 * @param rgr
 * @param rgrs
 * @return auto
 */
template <typename Ranger, typename... Rangers>
auto zip(Ranger rgr, Rangers... rgrs) {
  using cursor = zip_cursor<Ranger, Rangers...>;
  return ranger<cursor>(__lambda_244_25{cursor{}, rgr, rgrs...});
}

} // namespace transrangers

#undef TRANSRANGERS_HOT_MUTABLE
#undef TRANSRANGERS_HOT
#endif
