#pragma once
#include <tuple>
#include <type_traits>
#include <cstdint>
#include <cstdlib>

namespace mini {
namespace meta {

template <typename... Ts>
struct Type_List {};

template <typename T>
struct Callable_Traits : Callable_Traits<decltype(&T::operator())> {};

template <typename T, typename... Vs>
struct Callable_Traits<T (*)(Vs...)> {
    using param_list = Type_List<Vs...>;
};

template <typename T, typename... Vs>
struct Callable_Traits<T (*)(Vs...) noexcept> {
    using param_list = Type_List<Vs...>;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...)> {
    using param_list = Type_List<Vs...>;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) const> {
    using param_list = Type_List<Vs...>;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) noexcept> {
    using param_list = Type_List<Vs...>;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) const noexcept> {
    using param_list = Type_List<Vs...>;
};

template <typename T>
struct Element_Size : std::tuple_size<T> {
    // static constexpr auto value = std::tuple_size_v<T>;
};

template <typename T>
constexpr auto element_size = Element_Size<T>::value;

template <size_t I, typename T>
struct Element_Indexer {
    using type = std::tuple_element_t<I, T>;
};

template <size_t I, typename T>
using Element_Indexer_Type = typename Element_Indexer<I, T>::type;

} // namespace meta

} // namespace mini
