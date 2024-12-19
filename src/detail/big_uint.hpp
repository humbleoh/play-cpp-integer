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

template<std::unsigned_integral T, std::size_t N>
  requires (N > 0)
struct big_uint : std::array<T, (N + (sizeof(T) * CHAR_BIT) - 1) / (sizeof(T) * CHAR_BIT)> {
  using word_type = T;
  static constexpr std::size_t word_nbits = sizeof(word_type) * CHAR_BIT;
  static constexpr std::size_t word_hbytes = sizeof(word_type) * 2u;
  static constexpr word_type bitmask_msb = (1u << (N % word_nbits)) - 1u;
};

template<std::unsigned_integral T, std::size_t N1, std::size_t N2>
inline constexpr bool add(
  big_uint<T, (N1 > N2) ? N1 : N2>& u,
  const big_uint<T, N1>& v,
  const big_uint<T, N2>& w) noexcept
{
  T c = 0u;
  std::size_t i = 0u;
  constexpr std::size_t s = v.size() < w.size() ? v.size() : w.size();
  for ( ; i < s; ++i) {
    u[i] = v[i] + c;
    c = u[i] < c ? 1u : 0u;
    u[i] += w[i];
    if (u[i] < w[i]) {
      c++;
    }
  }

  if constexpr (N1 > N2) {
    for ( ; i < v.size(); ++i) {
      u[i] = v[i] + c;
      c = u[i] < c ? 1u : 0u;
    }
  } else {
    for ( ; i < w.size(); ++i) {
      u[i] = w[i] + c;
      c = u[i] < c ? 1u : 0u;
    }
  }

  if constexpr (u.bitmask_msb > 0) {
    u.back() &= u.bitmask_msb;
  }

  return c;
}

template<std::unsigned_integral T, std::size_t N1, std::size_t N2>
inline constexpr bool substract(
  big_uint<T, (N1 > N2) ? N1 : N2>& u,
  const big_uint<T, N1>& v,
  const big_uint<T, N2>& w) noexcept
{
  T b = 0u;
  std::size_t i = 0u;
  constexpr std::size_t s = v.size() < w.size() ? v.size() : w.size();
  for ( ; i < s; ++i) {
    u[i] = v[i] - b;
    b = (u[i] > std::numeric_limits<T>::max() - b) ? 1u : 0u;
    u[i] -= w[i];
    if (u[i] > std::numeric_limits<T>::max() - w[i]) {
      b++;
    }
  }

  if constexpr (v.size() > w.size()) {
    for ( ; i < v.size(); ++i) {
      u[i] = v[i] - b;
      b = (u[i] > std::numeric_limits<T>::max() - b) ? 1u : 0u;
    }
  } else {
    for ( ; i < w.size(); ++i) {
      u[i] = - b;
      b = (u[i] > std::numeric_limits<T>::max() - b) ? 1u : 0u;
      u[i] -= w[i];
      if (u[i] > std::numeric_limits<T>::max() - w[i]) {
        b++;
      }
    }
  }

  if constexpr (u.bitmask_msb > 0) {
    u.back() &= u.bitmask_msb;
  }

  return b;
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

#define ASSERT(pred) \
  do { \
    if constexpr(compile_time_assert((pred))) { \
      static_assert((pred)); \
    } else { \
      assert(pred); \
    } \
  } while (0)

template<std::unsigned_integral T, std::size_t N>
inline constexpr void from_string(big_uint<T, N>& u, std::string_view s) noexcept
{
  ASSERT((u.size() * (sizeof(T) << 1u)) > s.length());
//  std::size_t i = 0;
//  for (auto rit = std::cbegin(s); rit != std::cend(s); ++rit) {
//    char c = *rit;
//    T v;
//    if (c >= '0' && c <= '9') {
//      v = c - '0';
//    } else if (c >= 'a' && c <= 'z') {
//      v = c - 'a';
//    } else if (c >= 'A' && c <= 'Z') {
//      v = c - 'A';
//    } else {
//      ASSERT(false);
//    }
//    
//    u[i / u.word_hbytes] |= v << (i % u.word_hbytes * 4u);
//    i++;
//  }
}

template<std::unsigned_integral T, std::size_t N>
inline constexpr void to_string(std::string& s, const big_uint<T, N>& u)
{
  for (auto rit = std::crbegin(u); rit != std::crend(u); ++rit) {
    s.append(std::format("{1:0>{0}x}", sizeof(T) * 2, *rit));
  }
}

}
