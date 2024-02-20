#pragma once
#include "fwd.hpp"
#include "meta.hpp"
#include <type_traits>
#include <utility>

namespace mini {

template <typename T>
class Edge {
public:
    // @TODO(Marcus): we probably need to check for constness and all.
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;

    operator reference() { return value; }
    operator const_reference() const { return value; }

    reference get() { return value; }
    const_reference get() const { return value; }

    Edge() = default;

    template <typename... Args>
    Edge(Args&&... args) : value{ std::forward<Args&&>(args)... } {}

    template <typename TT, bool = std::is_same_v<TT, T> || std::is_convertible_v<TT, T>>
    Edge(const Edge<TT>& o) : value{ o.value } {}

    template <typename TT, bool = std::is_same_v<TT, T> || std::is_convertible_v<TT, T>>
    Edge(Edge<TT>&& o) : value{ std::move(o.value) } {}

    template <typename TT, bool = std::is_same_v<TT, T> || std::is_convertible_v<TT, T>>
    Edge& operator=(const Edge<TT>& o) {
        value = o.value;
        return *this;
    }

    template <typename TT, bool = std::is_same_v<TT, T> || std::is_convertible_v<TT, T>>
    Edge& operator=(Edge<TT>&& o) {
        value = std::move(o.value);
        return *this;
    }

    // actual types
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
    template <typename TT>
    friend class Edge;
    T value;
};

template <typename T>
class Relaxed_Edge { // should this be renamed as `Reference_Edge`?
public:
    template <typename TT>
    Relaxed_Edge(const Edge<TT>& o) :
        reference{ static_cast<const void*>(&o) }, converter{ +[](const void* p) -> T {
            // @TODO(Marcus): we should make the error message in this portion better.
            static_assert(std::is_convertible_v<TT, T>, "Must be convertible to other type");
            return static_cast<const Edge<TT>*>(p)->get();
        } } {}

    decltype(auto) get() const { return converter(reference); }

private:
    const void* reference;
    using Converter_Function = T (*)(const void*);
    Converter_Function converter;
};

// API
template <typename T>
Edge(T a) -> Edge<T>;

template <typename T>
Edge<T> edge(const T& o) {
    return Edge<T>(o);
}

template <typename T>
Edge<T> edge(T& o) {
    return Edge<T>(o);
}

template <typename T, typename... Args>
Edge<T> edge(Args&&... args) {
    return Edge<T>(std::forward<Args&&>(args)...);
}

template <typename T>
Edge<T> edge(T&& o) {
    return Edge<T>(std::move(o));
}

template <typename T, typename TT>
Relaxed_Edge<T> edge_cast(const Edge<TT>& o) {
    static_assert(std::is_convertible_v<TT, T>, "Type put in edge cast not convertible");
    return Relaxed_Edge<T>(o);
}

} // namespace mini
