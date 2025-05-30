#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <concepts>
#include <climits>
#include <cstddef>
#include <cstdint>
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

template<typename T>
concept digit_promotable = std::unsigned_integral<T>
  && (std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t>
    || std::same_as<T, std::uint32_t> || std::same_as<T, std::uint64_t>);

template<std::unsigned_integral T>
struct large_digit_from_digit;

#define DEFINE_LARGE_DIGIT_FROM_DIGIT(digit, large_digit) \
  template<> \
  struct large_digit_from_digit<digit> \
  { \
    using type = large_digit; \
  }

DEFINE_LARGE_DIGIT_FROM_DIGIT(std::uint8_t,  std::uint16_t);
DEFINE_LARGE_DIGIT_FROM_DIGIT(std::uint16_t, std::uint32_t);
DEFINE_LARGE_DIGIT_FROM_DIGIT(std::uint32_t, std::uint64_t);
DEFINE_LARGE_DIGIT_FROM_DIGIT(std::uint64_t, unsigned __int128);

template<typename T>
using large_digit = large_digit_from_digit<T>::type;

template<digit_promotable T>
constexpr large_digit<T> digit_multiply(T a, T b) noexcept
{
  return static_cast<large_digit<T>>(a) * b;
}

template<std::unsigned_integral T, std::size_t N>
constexpr void multiply(
  big_uint<T, N>& u,
  const big_uint<T, N>& v,
  const big_uint<T, N>& w) noexcept
{
  for (unsigned i = 0; i < std::size(v); ++i) {
    T c = 0u;
    for (unsigned j = 0, k = i; j < std::size(w) && k < std::size(u); ++j, ++k) {
      large_digit<T> r = digit_multiply(v[i], w[j]);
      r = r + u[k] + c;
      u[k] = std::numeric_limits<T>::max() & r;
      c = std::numeric_limits<T>::max() & (r >> (sizeof(T) << 3));
    }
  }
}

/*
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
