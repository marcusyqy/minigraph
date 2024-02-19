#pragma once
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

template <typename T, typename... As>
class Delegate<T(As...)> {
public:
    template <auto F_Ptr, typename R>
    Delegate(Connect_Arg<F_Ptr>, R& typed_reference) {
        connect<F_Ptr>(typed_reference);
    }

    template <typename R>
    Delegate(R& typed_reference) {
        connect(typed_reference);
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

    template <typename R>
    void connect(R& v) noexcept {
        reference = static_cast<const void*>(std::addressof(v));
        function  = [](const void* r, As... as) -> decltype(auto) {
            static_assert(std::is_invocable_r_v<T, R, As...>, "Must be invokable!!");
            constexpr auto is_const    = std::is_const_v<R>;
            using nonconst_return_type = decltype(std::declval<R>()(std::forward<As>(as)...));
            using const_return_type    = decltype(std::declval<std::add_const_t<R>>()(std::forward<As>(as)...));

            constexpr auto same_t_nonconst        = std::is_same_v<nonconst_return_type, T>;
            constexpr auto same_t_const           = std::is_same_v<const_return_type, T>;
            constexpr auto convertible_t_nonconst = std::is_convertible_v<nonconst_return_type, T>;

            using converted_ptr_type = std::conditional_t<
                !is_const && (same_t_nonconst || (!same_t_const && convertible_t_nonconst)),
                R*,
                const R*>;

            auto& callable = *static_cast<converted_ptr_type>(const_cast<void*>(r));
            if constexpr (same_t_const || same_t_nonconst) {
                return std::invoke(callable, std::forward<As>(as)...);
            } else { // do conversion here.
                return T{ std::invoke(callable, std::forward<As>(as)...) };
            }
        };
    }

    template <auto F_Ptr, typename R>
    void connect(R& v) noexcept {
        reference = static_cast<const void*>(std::addressof(v));
        function  = [](const void* r, As... as) -> decltype(auto) {
            static_assert(
                std::is_invocable_r_v<T, decltype(F_Ptr), R, As...>,
                "Connect called with function that does not satisfy func signature of `Delegate`");

            constexpr auto is_const    = std::is_const_v<R>;
            using nonconst_return_type = decltype(std::invoke(F_Ptr, std::declval<R>(), std::forward<As>(as)...));
            using const_return_type =
                decltype(std::invoke(F_Ptr, std::declval<std::add_const_t<R>>(), std::forward<As>(as)...));

            constexpr auto same_t_nonconst        = std::is_same_v<nonconst_return_type, T>;
            constexpr auto same_t_const           = std::is_same_v<const_return_type, T>;
            constexpr auto convertible_t_nonconst = std::is_convertible_v<nonconst_return_type, T>;

            using converted_ptr_type = std::conditional_t<
                !is_const && (same_t_nonconst || (!same_t_const && convertible_t_nonconst)),
                R*,
                const R*>;

            auto& callable = *static_cast<converted_ptr_type>(const_cast<void*>(r));
            if constexpr (same_t_const || same_t_nonconst) {
                return std::invoke(F_Ptr, callable, std::forward<As>(as)...);
            } else { // do conversion here.
                return T{ std::invoke(F_Ptr, callable, std::forward<As>(as)...) };
            }
        };
    }

    void reset() noexcept {
        reference = nullptr;
        function  = nullptr;
    }

private:
    const void* reference             = nullptr;
    T (*function)(const void*, As...) = nullptr;
};

} // namespace mini