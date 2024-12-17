#pragma once

#include "details/big_uint.hpp"

#include <algorithm>
#include <array>
#include <type_traits>

namespace big_uint {

template<size_t Nbits>
struct big_uint {
  template<size_t N1, size_t N2>
  friend constexpr auto operator+(const big_uint<N1>& u, const big_uint<N2>& v)
    -> big_uint<(N1 > N2) ? N1 : N2>;

  using word_type_ = unsigned int;
  static constexpr size_t word_size_ = sizeof(word_type_);
  static constexpr size_t digit_count_ = Nbits / word_size_ + \
    (((1u << word_size_) - 1) & Nbits) > 0 ? 1 : 0;
  std::array<word_type_, digit_count_> digits_;
};

template<size_t N1, size_t N2, size_t N3>
inline constexpr void add(big_uint<N3>& w, const big_uint<N1>& u, const big_uint<N2>& v)
{
  static_assert(N3 >= N1);
  static_assert(N3 >= N2);
  static_assert(N1 >= N2);

  typename std::remove_reference_t<decltype(v)>::word_type_ carry = 0;
  for (int i = 0; i < v.digits_.size(); ++i) {
    w.digits_[i] = u.digits_[i] + carry;
    //carry 
  }
}

template<size_t N1, size_t N2>
inline constexpr auto operator+(const big_uint<N1>& u, const big_uint<N2>& v)
  -> big_uint<(N1 > N2) ? N1 : N2>
{
  if constexpr (N1 > N2) {
    big_uint<N1> w { 0 };
    add(w, u, v);
    return w;
  } else {
    big_uint<N2> w { 0 };
    add(w, v, u);
    return w;
  }
}

}
