#pragma once
#include "edge.hpp"
#include "meta.hpp"
#include <tuple>

namespace mini {

namespace meta {
namespace detail {

template <typename>
struct Type_List_To_Tuple_Impl;

// @TODO: support more than just this tuple
template <typename... Ts>
struct Type_List_To_Tuple_Impl<Type_List<Ts...>> {
    using type = std::tuple<Ts...>;
};
template <typename T>
using Type_List_To_Tuple = typename Type_List_To_Tuple_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Relaxed_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Relaxed_Edge_Impl<Type_List<Ts...>> {
    using type = Type_List<Relaxed_Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Relaxed_Edge = typename Decorate_Type_List_With_Relaxed_Edge_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Edge_Impl<Type_List<Ts...>> {
    using type = Type_List<Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Edge = typename Decorate_Type_List_With_Edge_Impl<T>::type;

template <typename T>
using Params = typename meta::Callable_Traits<T>::param_list;

template <typename T>
using Returns = typename meta::Callable_Traits<T>::return_list;

template <typename T>
using Return_Type = typename meta::Callable_Traits<T>::return_type;

template <typename T>
using Node_Input = Type_List_To_Tuple<Decorate_Type_List_With_Relaxed_Edge<Params<T>>>;

template <typename T>
using Node_Output = Type_List_To_Tuple<Decorate_Type_List_With_Edge<Returns<T>>>;

} // namespace detail
} // namespace meta

template <typename T>
class Node {
public:
    // @TODO: add outputs intitalization.
    template <typename... Args>
    Node(meta::detail::Node_Input<T> o, Args&&... args) :
        callable{ std::forward<Args&&>(args)... }, inputs{ o }, outputs{} {}

    void operator()() {
        apply(
            std::make_index_sequence<meta::detail::Params<T>::size>{},
            std::make_index_sequence<meta::detail::Returns<T>::size>{});
    }

    meta::detail::Node_Output<T>& edges() noexcept { return outputs; }
    const meta::detail::Node_Output<T>& edges() const noexcept { return outputs; }

private:
    template <size_t... Is, size_t... Os>
    void apply(std::index_sequence<Is...>, std::index_sequence<Os...>) {
        auto immediate = callable(std::get<Is>(inputs).get()...);
        // @TODO: we have to change
        if constexpr (meta::is_tuple_like<meta::detail::Return_Type<T>>) {
            (..., [&] { meta::get<Os>(outputs) = meta::get<Os>(immediate); }());
        } else {
            meta::get<0>(outputs) = immediate;
        }
    }

private:
    T callable;
    meta::detail::Node_Input<T> inputs;
    meta::detail::Node_Output<T> outputs;
};

/// make a tuple-like check for the struct
/// for default initialized edges.
template <typename T, typename... Args>
decltype(auto) node(meta::detail::Node_Input<T> o, Args&&... args) {
    return Node<T>{ o, std::forward<Args&&>(args)... };
}

} // namespace mini
