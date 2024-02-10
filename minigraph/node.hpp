#pragma once
#include "edge.hpp"
#include "meta.hpp"
#include <tuple>

namespace mini {

namespace meta {
namespace detail {

template <typename>
struct Type_List_To_Tuple_Impl;
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
using Node_Input = Type_List_To_Tuple<Decorate_Type_List_With_Relaxed_Edge<Params<T>>>;

template <typename T>
using Node_Output = Type_List_To_Tuple<Decorate_Type_List_With_Edge<Params<T>>>;

} // namespace detail
} // namespace meta

template <typename T>
class Node {
public:
    template<typename... Args>
    Node(meta::detail::Node_Input<T> o, Args&&... args) : callable{std::forward<Args&&>(args)...}, inputs{o}, outputs{} {
    }

    void operator()() {
    }

    meta::detail::Node_Output<T>& edges() const noexcept { return outputs;}

private:
    T callable;
    meta::detail::Node_Input<T> inputs;
    meta::detail::Node_Output<T> outputs;
};

// make a tuple-like check for the struct
template <typename T, typename... Args>
decltype(auto) node(meta::detail::Node_Input<T> o, Args&&... args) {
    return Node<T>{o, std::forward<Args&&>(args)...};
}

} // namespace mini
