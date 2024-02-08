#pragma once

namespace mini {
namespace meta {

template<typename... Ts>
struct Type_List {};

template<typename>
struct Callable_Extractor;

template<typename T, typename ... Vs>
struct Callable_Extractor<T(*)(Vs...)> {
    using param_list = Type_List<Vs...>;
};

template<typename T, typename ... Vs>
struct Callable_Extractor<T(*)(Vs...) noexcept> {
    using param_list = Type_List<Vs...>;
};

template<typename T, typename R, typename ... Vs>
struct Callable_Extractor<T(R::*)(Vs...)> {
    using param_list = Type_List<Vs...>;
};

template<typename T, typename R, typename ... Vs>
struct Callable_Extractor<T(R::*)(Vs...) const > {
    using param_list = Type_List<Vs...>;
};

template<typename T, typename R, typename ... Vs>
struct Callable_Extractor<T(R::*)(Vs...) noexcept> {
    using param_list = Type_List<Vs...>;
};

template<typename T, typename R, typename ... Vs>
struct Callable_Extractor<T(R::*)(Vs...) const noexcept> {
    using param_list = Type_List<Vs...>;
};


template<typename T>
struct Callable_Metadata {

};



}
}
