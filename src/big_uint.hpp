#pragma once

#include "detail/big_uint.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string_view>
#include <type_traits>

namespace big_uint {

template<std::unsigned_integral DigitType, std::size_t N>
struct big_uint : detail::big_uint::big_uint<DigitType, N>
{
};

template<std::unsigned_integral DigitType, std::size_t N>
constexpr big_uint<DigitType, N> operator+(
  const big_uint<DigitType, N>& u,
  const big_uint<DigitType, N>& v) noexcept
{
  big_uint<DigitType, N> w;
  detail::big_uint::add(w, u, v);
  return w;
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
template<std::unsigned_integral DigitType, std::size_t N>
constexpr big_uint<DigitType, N> from_string(std::string_view s) noexcept
{
  auto v = big_uint<DigitType, N>{ 0 };
  detail::big_uint::from_string(v, s);
  return v;
}
}

template<std::unsigned_integral DigitType, std::size_t N>
struct std::formatter<big_uint::big_uint<DigitType, N>>
{
  constexpr auto parse(std::format_parse_context& context)
  {
    return context.begin();
  }
  
  auto format(const big_uint::big_uint<DigitType, N>& v, std::format_context& context) const
  {
    std::string s;
    detail::big_uint::to_string(s, v);
    return std::format_to(context.out(), "{}", s);
  }
};
