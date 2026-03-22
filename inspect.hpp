// inspect.hpp
#pragma once
#include <string_view>

template <char... Cs> struct ShowChars;

template <std::string_view const &S, std::size_t... Is>
auto inspect_impl(std::index_sequence<Is...>) {
  return ShowChars<S[Is]...>{};
}

template <std::string_view const &S> auto inspect() {
  return inspect_impl<S>(std::make_index_sequence<S.size()>{});
}