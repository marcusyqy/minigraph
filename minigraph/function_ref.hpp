#pragma once
#include <cassert>
#include <iostream>
#include <utility>

namespace mini {

template <typename>
class Function_Ref;

template <typename T, typename... As>
class Function_Ref<T(As...)> {
public:
    template <typename R>
    Function_Ref(R& typed_reference) :
        reference(static_cast<const void*>(std::addressof(typed_reference))),
        function([](const void* r, As... as) -> decltype(auto) {
            static_assert(std::is_invocable_r_v<T, R, As...>, "Must be invokable!!");
            using nonconst_return_type = decltype(std::declval<R>()(std::forward<As>(as)...));
            using const_return_type    = decltype(std::declval<std::add_const_t<R>>()(std::forward<As>(as)...));

            constexpr auto same_t_nonconst        = std::is_same_v<nonconst_return_type, T>;
            constexpr auto same_t_const           = std::is_same_v<const_return_type, T>;
            constexpr auto convertible_t_nonconst = std::is_convertible_v<nonconst_return_type, T>;
            // constexpr auto convertible_t_const = std::is_convertible_v<const_return_type, T>;
            //       (we don't need this but we will leave it for now).

            using converted_ptr_type =
                std::conditional_t<same_t_nonconst || (!same_t_const && convertible_t_nonconst), R*, const R*>;

            auto& callable = *static_cast<converted_ptr_type>(const_cast<void*>(r));
            if constexpr (same_t_const || same_t_nonconst) {
                return callable(std::forward<As>(as)...);
            } else { // do conversion here.
                return T{ callable(std::forward<As>(as)...) };
            }
        }) {}

    template <typename R>
    Function_Ref(const R& typed_reference) :
        reference(static_cast<const void*>(std::addressof(typed_reference))),
        function([](const void* r, As... as) -> decltype(auto) {
            const R& callable = *static_cast<const R*>(r);
            return callable(std::forward<As>(as)...);
        }) {}

    template <typename R>
    Function_Ref(T (*fn)(As...)) : reference(nullptr), function(fn) {}

    Function_Ref() = default;

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        static_assert(std::is_invocable_r_v<T, decltype(function), void*, Args&&...>, "Must be callable with Args");
        assert(function);
        return function(reference, std::forward<Args&&>(args)...);
    }

private:
    const void* reference             = nullptr;
    T (*function)(const void*, As...) = nullptr;
};

} // namespace mini