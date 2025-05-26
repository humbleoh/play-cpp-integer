#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <concepts>
#include <climits>
#include <cstddef>
#include <format>
#include <numeric>
#include <string_view>
#include <type_traits>

namespace detail::big_uint {

consteval std::size_t ceil(std::size_t value, std::size_t base)
{
  return (value + base - 1) / base;
}

template<std::unsigned_integral T, std::size_t N>
struct big_uint : std::array<T, N / (sizeof(T) * CHAR_BIT)>
{
  using digit_type = T;
  static constexpr std::size_t digit_nbits = sizeof(T) * CHAR_BIT;
  static_assert(N % digit_nbits == 0);
  static constexpr std::size_t digit_nhbytes = (sizeof(T) << 1u);
};

template<std::unsigned_integral T, std::size_t N>
constexpr bool msb(const big_uint<T, N>& u) noexcept
{
  constexpr unsigned pos_msb = u.digit_nbits - 1;
  return (u.back() >> pos_msb & 0x1u);
}

template<std::unsigned_integral T, std::size_t N>
constexpr bool add_digit(
  big_uint<T, N>& u,
  const big_uint<T, N>& v,
  T w) noexcept
{
  T c = w;
  for (unsigned i = 0u; i < v.size() && c > 0; ++i) {
    T temp = v[i];
    u[i] = temp + c;
    c = u[i] < temp;
  }

  return c;
}

template<std::unsigned_integral T, std::size_t N>
constexpr void two_complement(
  big_uint<T, N>& u,
  const big_uint<T, N>& v) noexcept
{
  for (unsigned i = 0u; i < v.size(); ++i) {
    u[i] = ~v[i];
  }

  add_digit(u, u, 1u);
}

template<std::unsigned_integral T, std::size_t N>
constexpr bool add(
  big_uint<T, N>& u,
  const big_uint<T, N>& v,
  const big_uint<T, N>& w) noexcept
{
  T c = 0u;
  for (unsigned i = 0u; i < u.size(); ++i) {
    u[i] = v[i] + w[i] + c;
    c = (!c && (u[i] < v[i])) || (c && (u[i] <= v[i]));
  }

  return c;
}

template<std::unsigned_integral T, std::size_t N>
constexpr bool substract(
  big_uint<T, N>& u,
  const big_uint<T, N>& v,
  const big_uint<T, N>& w) noexcept
{
  T b = 0u;
  for (unsigned i = 0u ; i < v.size(); ++i) {
    u[i] = v[i] - w[i] - b;
    b = (!b && u[i] > v[i]) || (b && u[i] >= v[i]);
  }

  return b;
}

/*
template<std::unsigned_integral T, std::size_t N1, std::size_t N2>
inline constexpr bool multiply(
  big_uint<T, (N1 > N2) ? N1 : N2>& u,
  const big_uint<T, N1>& v,
  const big_uint<T, N2>& w) noexcept
{
  return false;
}

template<std::unsigned_integral T, std::size_t N>
inline constexpr void complement(big_uint<T, N>& u, const big_uint<T, N>& v) noexcept
{
  for (std::size_t i = 0u; i < v.size(); ++i) {
    u[i] = ~v[i];
  }

  if constexpr (u.bitmask_msb > 0) {
    u.back() &= u.bitmask_msb;
  }
}

inline constexpr bool compile_time_assert(bool pred) noexcept
{
  return true;
}

*/
#define ASSERT(pred) \
  do { \
    if constexpr (std::is_constant_evaluated()) { \
      static_assert((pred)); \
    } else { \
      assert(pred); \
    } \
  } while (0)

template<std::unsigned_integral T, std::size_t N>
constexpr void from_string(big_uint<T, N>& u, std::string_view s) noexcept
{
  unsigned i = 0;
  for (auto rit = std::crbegin(s); rit != std::crend(s); ++rit) {
    char c = *rit;
    T v;
    if (c >= '0' && c <= '9') {
      v = c - '0';
    } else if (c >= 'a' && c <= 'z') {
      v = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'Z') {
      v = c - 'A' + 10;
    } else {
      // how?
    }
    
    u[i / u.digit_nhbytes] |= v << ((i % u.digit_nhbytes) * 4u);
    i++;
  }
}

template<std::unsigned_integral T, std::size_t N>
constexpr void to_string(std::string& s, const big_uint<T, N>& u)
{
  for (auto rit = std::crbegin(u); rit != std::crend(u); ++rit) {
    s.append(std::format("{1:0>{0}x}", sizeof(T) << 1u, *rit));
  }
}

}
