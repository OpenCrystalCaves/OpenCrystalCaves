#pragma once

#include <array>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>

namespace misc
{

template<typename... T>
constexpr auto make_array(T&&... values) -> std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(T)>
{
  return std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(T)>{std::forward<T>(values)...};
}

template<typename T>
T random(T min, T max)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<T> dis(min, max);
  return dis(gen);
}

void open_url(const std::string& url);

}
