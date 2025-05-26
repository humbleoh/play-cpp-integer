#include "big_int.hpp"
#include "big_uint.hpp"

#include <iostream>
#include <format>
#include <string_view>

#include <stdint.h>

using namespace std::literals;

int main(int argc, char *arg[])
{
  constexpr big_int::big_int<uint32_t, 128> u { 5 };
  constexpr big_int::big_int<uint32_t, 128> v { 1 };
  constexpr auto w = u + v;
  std::cout << std::format("{}", u) << std::endl;
  std::cout << std::format("{}", v) << std::endl;
  std::cout << std::format("{}", w) << std::endl;
  constexpr auto y = big_int::from_string<uint32_t, 128>("-123456789abcdef123456fafafafff"sv);
  constexpr auto z = big_int::from_string<uint32_t, 128>("-123456789abcdef123456fafafafff"sv);
  constexpr auto x = y + z;
  std::cout << std::format("{}", x) << std::endl;

  constexpr big_uint::big_uint<uint32_t, 128> uu { 5 };
  constexpr big_uint::big_uint<uint32_t, 128> uv { 1 };
  constexpr auto uw = uu + uv;
  std::cout << std::format(" {}", uu) << std::endl;
  std::cout << std::format(" {}", uv) << std::endl;
  std::cout << std::format(" {}", uw) << std::endl;
  constexpr auto uy = big_uint::from_string<uint32_t, 128>("123456789abcdef123456fafafafff"sv);
  constexpr auto uz = big_uint::from_string<uint32_t, 128>("123456789abcdef123456fafafafff"sv);
  constexpr auto ux = uy + uz;
  std::cout << std::format(" {}", uy + uz) << std::endl;
  constexpr auto uyy = big_uint::from_string<uint32_t, 128>("ffffffffffffffffffffffffffffff"sv);
  constexpr auto uzz = big_uint::from_string<uint32_t, 128>("ffffffffffffffffffffffffffffff"sv);
  constexpr auto uxx = uyy + uzz;
  std::cout << std::format(" {}", uxx) << std::endl;

  std::cout << "==" << std::endl;
  constexpr auto a = big_uint::from_string<uint32_t, 128>("86f6050d979544fe39e7142446a40b3"sv);
  constexpr auto b = big_uint::from_string<uint32_t, 128>("ab5a0c4d00312f392c0edc2878d6d8a"sv);
  constexpr auto c = b - a;
  constexpr auto d = a - b;
  constexpr auto e = big_uint::two_complement(d);
  std::cout << std::format(" {}", a) << std::endl;
  std::cout << std::format(" {}", b) << std::endl;
  std::cout << std::format(" {}", c) << std::endl;
  std::cout << std::format(" {}", d) << std::endl;
  std::cout << std::format(" {}", e) << std::endl;
  /*
  std::cout << std::format("{} {} {} {} {:b}", u[0], u.size(), u.word_nbits,
    u.word_hbytes, w.bitmask_msb) << std::endl;
  std::cout << std::format("{} {}", w[0], w.size()) << std::endl;
  constexpr auto x = u - v;
  std::cout << std::format("{}", x) << std::endl;
  constexpr auto y = v - u;
  std::cout << std::format("{}", y) << std::endl;
  z = z + z + z + z;
  std::cout << std::format("{}", z) << std::endl;
  */
  return 0;
}
