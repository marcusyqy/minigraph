#pragma once
#include <type_traits>
#include <utility>

namespace mini {

template <typename T>
class Edge {
public:
    // @TODO(Marcus): we probably need to check for constness and all.
    using reference       = T&;
    using const_reference = const T&;

    operator reference() { return value; }
    operator const_reference() const { return value; }

    reference get() { return value; }
    const_reference get() const { return value; }

    Edge(const T& o) : value{ o } {}
    Edge(T&& o) : value{ std::move(o) } {}
    Edge& operator=(const T& o) {
        value = o;
        return *this;
    }
    Edge& operator=(T&& o) {
        value = std::move(o);
        return *this;
    }

private:
    T value;
};

template <typename T>
class Relaxed_Edge { // should this be renamed as `Reference_Edge`?
    using Converter_Function = T (*)(const void*);
public:
    template <typename TT>
    Relaxed_Edge(const Edge<TT>& o) :
        reference { static_cast<const void*>(&o) },
        converter { +[](const void* p) -> T {
            static_assert(std::is_convertible_v<TT, T>, "Must be convertible to other type");
            return static_cast<const Edge<TT>*>(p)->get();
        }} {
    }

    T get() const { return converter(reference); }

private:
    const void* reference;
    Converter_Function converter;
};

} // namespace mini