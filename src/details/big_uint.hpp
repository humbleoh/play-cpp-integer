#include <algorithm>
#include <array>
#include <concepts>
#include <climits>
#include <cstddef>
#include <type_traits>

namespace details::big_uint {

template<std::unsigned_integral Word, std::size_t Nbits>
struct big_uint {
  using value_type = Word;
  static constexpr std::size_t value_type_nbits = sizeof(value_type) * CHAR_BIT;
  static constexpr value_type bitmask_msb = (1u << (Nbits % value_type_nbits)) - 1u;
  std::array<Word, Nbits / value_type_nbits + bitmask_msb ? 1 : 0> digits;

  constexpr value_type& operator[](std::size_t i)
  {
    return digits[i];
  }
};

template<std::unsigned_integral T, std::size_t N1, std::size_t N2>
constexpr bool add(big_uint<T, N1>& u, const big_uint<T, N1>& v, const big_uint<T, N2>& w) noexcept
  requires (N1 >= N2)
{
  T c = 0u;
  std::size_t i;
  for (i = 0; i < w.digits.size(); ++i) {
    u[i] = v[i] + c;
    c = u[i] < c ? 1u : 0u;
    u[i] += w[i];
    if (u[i] < w[i]) {
      c++;
    }
  }

  if constexpr (N1 > N2) {
    for (; i < v.digits.size(); ++i) {
      u[i] = v[i] + c;
      c = u[i] < c ? 1u : 0u;
    }
  }

  if constexpr (u.bitmask_msb > 0) {
    u.digits.back() &= u.bitmask_msb;
  }

  return c;
}

template<std::unsigned_integral T, std::size_t N1, std::size_t N2>
constexpr bool substract(big_uint<T, N1>& u, const big_uint<T, N1>& v, const big_uint<T, N2>& w) noexcept
  requires (N1 >= N2)
{
  T b = 0;
  return b;
}

template<std::unsigned_integral T, std::size_t N>
constexpr void complement(big_uint<T, N>& u, const big_uint<T, N>& v) noexcept
{
  for (std::size_t i = 0u; i < v.digits.size(); ++i) {
    u[i] = ~v[i];
  }

  if constexpr (u.bitmask_msb > 0) {
    u.digits.back() &= u.bitmask_msb;
  }
}

}
