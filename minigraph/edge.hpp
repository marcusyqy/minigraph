#pragma once
#include "delegate.hpp"
#include "fwd.hpp"
#include "meta.hpp"
#include <type_traits>
#include <utility>
#include <vector>

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
        broadcast();
        return *this;
    }

    template <typename TT, bool = std::is_same_v<TT, T> || std::is_convertible_v<TT, T>>
    Edge& operator=(Edge<TT>&& o) {
        o.reset();
        value = std::move(o.value);
        broadcast();
        return *this;
    }

    // actual types
    Edge(const T& o) : value{ o } {
        // don't need to broadcast this since this is initial value
        // broadcast();
    }

    Edge(T&& o) : value{ std::move(o) } {
        // don't need to broadcast this since this is initial value
        // broadcast();
    }
    Edge& operator=(const T& o) {
        value = o;
        broadcast();
        return *this;
    }
    Edge& operator=(T&& o) {
        value = std::move(o);
        broadcast();
        return *this;
    }

    void listen(mini::Delegate<void()> delegate) { on_changed_listeners.emplace_back(std::move(delegate)); }

    void broadcast() {
        for (auto& on_changed : on_changed_listeners) {
            on_changed();
        }
    }

    void reset() { on_changed_listeners.clear(); }

private:
    template <typename TT>
    friend class Edge;
    T value;

    // we can change this to not vector.
    std::vector<Delegate<void()>> on_changed_listeners;
};

template <typename T>
class Relaxed_Edge { // should this be renamed as `Reference_Edge`?
public:
    template <typename TT>
    Relaxed_Edge(Edge<TT>& o) :
        reference{ static_cast<void*>(&o) }, converter{ +[](void* p) -> T {
            // @TODO(Marcus): we should make the error message in this portion better.
            static_assert(std::is_convertible_v<TT, T>, "Must be convertible to other type");
            return static_cast<const Edge<TT>*>(p)->get();
        } },
        listen_fn(+[](void* p, mini::Delegate<void()> delegate) {
            static_cast<Edge<TT>*>(p)->listen(std::move(delegate));
        }) {}

    decltype(auto) get() const {
        assert(converter);
        return converter(reference);
    }

    void listen(mini::Delegate<void()> delegate) {
        assert(listen_fn);
        listen_fn(reference, std::move(delegate));
    }

private:
    void* reference;
    using Converter_Function = T (*)(void*);
    using Listen_Function    = void (*)(void*, mini::Delegate<void()>);

    Converter_Function converter;
    Listen_Function listen_fn;
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
