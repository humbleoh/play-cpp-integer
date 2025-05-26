#pragma once

#include "big_uint.hpp"

#include <algorithm>
#include <format>
#include <string_view>
#include <type_traits>

namespace big_int {

template<std::unsigned_integral T, std::size_t N>
struct big_int : big_uint::big_uint<T, N>
{
  constexpr bool sign() const noexcept
  {
    return detail::big_uint::msb(*this);
  }
};

template<std::unsigned_integral T, std::size_t N>
constexpr big_int<T, N> operator+(
  const big_int<T, N>& u,
  const big_int<T, N>& v) noexcept
{
  using bint = big_int<T, N>;
  using buint = big_uint::big_uint<T, N>;
  auto& up = static_cast<const buint&>(u);
  auto& vp = static_cast<const buint&>(v);
  return static_cast<bint>(up + vp);
}

/*
template<std::unsigned_integral T, std::size_t N>
template<std::size_t Np>
inline constexpr auto big_uint<T, N>::operator-(
  const big_uint<T, Np>& u) const
  -> big_uint<T, (N > Np) ? N : Np>
{
  auto w = big_uint<T, (N > Np) ? N : Np>{ 0 };
  detail::big_uint::substract(w, *this, u);
  return w;
}

template<std::unsigned_integral T, std::size_t N>
template<std::size_t Np>
inline constexpr auto big_uint<T, N>::operator*(
  const big_uint<T, Np>& u) const
  -> big_uint<T, (N > Np) ? N : Np>
{
  auto w = big_uint<T, (N > Np) ? N : Np>{ 0 };
  detail::big_uint::multiply(w, *this, u);
  return w;
}

*/
template<std::unsigned_integral T, std::size_t N>
constexpr big_int<T, N> from_string(std::string_view s) noexcept
{
  auto v = big_int<T, N>{ 0 };
  bool sign = (s[0] == '-');
  detail::big_uint::from_string(v, sign ? s.substr(1) : s);
  if (sign)
    detail::big_uint::two_complement(v, v);
  return v;
}
}

template<std::unsigned_integral T, std::size_t N>
struct std::formatter<big_int::big_int<T, N>>
{
  constexpr auto parse(std::format_parse_context& context)
  {
    return context.begin();
  }
  
  auto format(const big_int::big_int<T, N>& v, std::format_context& context) const
  {
    std::string s;
    char sign = v.sign() ? '-' : '+';
    if (v.sign()) {
      big_uint::big_uint<T, N> w;
      detail::big_uint::two_complement(w, v);
      detail::big_uint::to_string(s, w);
    } else {
      detail::big_uint::to_string(s, v);
    }
    return std::format_to(context.out(), "{}{}", sign, s);
  }
};
