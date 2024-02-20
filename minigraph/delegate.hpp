#pragma once
#include "fwd.hpp"
#include <cassert>
#include <functional>
#include <iostream>
#include <utility>

namespace mini {

template <typename>
class Delegate;

template <auto>
struct Connect_Arg {};

template <auto X>
constexpr auto connect = Connect_Arg<X>{};

namespace detail {
struct Dummy {};

template <bool, typename... Ts>
struct Conditional_Return_From_Const_Expr_Impl {
    using type = Dummy;
};

template <typename... Ts>
struct Conditional_Return_From_Const_Expr_Impl<true, Ts...> {
    using type = decltype(std::invoke(std::declval<Ts>()...));
};

template <bool B, typename... Ts>
using Conditional_Return_From_Const_Expr = typename Conditional_Return_From_Const_Expr_Impl<B, Ts...>::type;
} // namespace detail

template <typename T, typename... As>
class Delegate<T(As...)> {
public:
    template <auto F_Ptr, typename R>
    Delegate(Connect_Arg<F_Ptr>, R& typed_reference) {
        connect<F_Ptr>(typed_reference);
    }

    template <auto F_Ptr>
    Delegate(Connect_Arg<F_Ptr>) {
        connect<F_Ptr>();
    }

    template <typename R>
    Delegate(R& typed_reference) {
        connect(typed_reference);
    }

    template <typename R>
    void connect(R& v) noexcept {
        connect_impl<>(v);
    }

    template <auto F_Ptr, typename R>
    void connect(R& v) noexcept {
        connect_impl<F_Ptr>(v);
    }

    template <auto F_Ptr>
    void connect() {
        reference = nullptr;
        function  = [](const void*, As... as) { return std::invoke(F_Ptr, std::forward<As>(as)...); };
    }

    template <typename R>
    Delegate(T (*fn)(As...)) : function(fn) {}

    Delegate()                                 = default;
    Delegate(const Delegate& o)                = default;
    Delegate(Delegate&& o) noexcept            = default;
    Delegate& operator=(const Delegate& o)     = default;
    Delegate& operator=(Delegate&& o) noexcept = default;
    ~Delegate()                                = default;

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        static_assert(std::is_invocable_r_v<T, decltype(function), void*, Args&&...>, "Must be callable with Args");
        assert(function);
        return function(reference, std::forward<Args&&>(args)...);
    }

    operator bool() const noexcept { return function != nullptr; }
    [[nodiscard]] bool empty() const noexcept { return static_cast<bool>(*this); }
    const void* data() const noexcept { return reference; }
    decltype(auto) fn() const noexcept { return function; }

    void reset() noexcept {
        reference = nullptr;
        function  = nullptr;
    }

private:
    template <auto... F, typename R>
    void connect_impl(R& v) noexcept {
        reset();

        constexpr auto is_correct_signature = std::is_invocable_r_v<T, decltype(F)..., R, As...>;
        constexpr auto is_not_function_ptr  = is_correct_signature && !std::is_pointer_v<std::decay_t<R>>;
        static_assert(
            is_correct_signature,
            "Connect called with function that does not satisfy func signature of `Delegate`");
        static_assert(
            is_not_function_ptr,
            "Function pointers are not defined when cast to `void*`. Use connect<&F> instead");

        if constexpr (is_not_function_ptr) {
            reference = static_cast<const void*>(std::addressof(v));
            function  = [](const void* r, As... as) -> decltype(auto) {
                constexpr auto is_const = std::is_const_v<R>;
                constexpr bool is_invokable_const =
                    std::is_invocable_r_v<T, decltype(F)..., std::add_const_t<R>, As...>;

                using nonconst_return_type = decltype(std::invoke(F..., std::declval<R>(), std::forward<As>(as)...));
                constexpr auto same_t_nonconst        = std::is_same_v<nonconst_return_type, T>;
                constexpr auto convertible_t_nonconst = std::is_convertible_v<nonconst_return_type, T>;

                using const_return_type =
                    detail::Conditional_Return_From_Const_Expr<is_invokable_const, decltype(F)..., R, As...>;
                constexpr auto same_t_const = std::is_same_v<const_return_type, T>;
                static_assert(
                    !is_const || is_invokable_const,
                    "The only function signature that matches is non const but supplied object to delegate is const");

                using converted_ptr_type = std::conditional_t<
                    !is_const && (same_t_nonconst || (!same_t_const && convertible_t_nonconst)),
                    R*,
                    const R*>;

                auto& callable = *static_cast<converted_ptr_type>(const_cast<void*>(r));
                if constexpr (same_t_const || same_t_nonconst) {
                    return std::invoke(F..., callable, std::forward<As>(as)...);
                } else { // do conversion here.
                    return T{ std::invoke(F..., callable, std::forward<As>(as)...) };
                }
            };
        }
    }

private:
    const void* reference             = nullptr;
    T (*function)(const void*, As...) = nullptr;
};

} // namespace mini