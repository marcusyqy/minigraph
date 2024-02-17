#pragma once
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <type_traits>

namespace mini {
namespace meta {

template <typename... Ts>
struct Type_List {
    static constexpr auto size = sizeof...(Ts);
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

template <typename T>
struct Is_Tuple_Like : std::false_type {};

template <typename... Ts>
struct Is_Tuple_Like<std::tuple<Ts...>> : std::true_type {};

template <typename T>
constexpr auto is_tuple_like = Is_Tuple_Like<T>::value;

// @TODO: make it work for anything that can use above Element_Indexer and etc.
template <typename T, typename = std::enable_if_t<!is_tuple_like<T>, void>>
struct Decompose_Into_Type_List_Impl {
    using type = Type_List<T>;
};

template <typename... Ts>
struct Decompose_Into_Type_List_Impl<std::tuple<Ts...>, void> {
    using type = Type_List<Ts...>;
};

template <typename T>
using Decompose_Into_Type_List = typename Decompose_Into_Type_List_Impl<T>::type;

template <typename T>
struct Callable_Traits : Callable_Traits<decltype(&T::operator())> {};

template <typename T, typename... Vs>
struct Callable_Traits<T (*)(Vs...)> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <typename T, typename... Vs>
struct Callable_Traits<T (*)(Vs...) noexcept> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...)> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) const> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) noexcept> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <typename T, typename R, typename... Vs>
struct Callable_Traits<T (R::*)(Vs...) const noexcept> {
    using param_list  = Type_List<Vs...>;
    using return_list = Decompose_Into_Type_List<T>;
    using return_type = T;
};

template <size_t N, typename T>
decltype(auto) get(T&& t) {
    return std::get<N>(std::forward<T&&>(t));
}

} // namespace meta

} // namespace mini
