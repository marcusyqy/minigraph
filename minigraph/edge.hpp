#pragma once
#include "delegate.hpp"
#include "fwd.hpp"
#include "meta.hpp"
#include <type_traits>
#include <utility>
#include <vector>

namespace mini {

namespace detail {

template <typename T, typename = void>
class Edge_Storage {
public:
    template <typename... TArgs>
    Edge_Storage(TArgs&&... args) : value{ std::forward<TArgs&&>(args)... } {}
    Edge_Storage(const T& o) : value{ o } {}
    Edge_Storage(T&& o) : value{ std::move(o) } {}
    Edge_Storage& operator=(const T& o) {
        std::destroy_at(std::addressof(value));
        new (&value) T{ o };
        return *this;
    }
    Edge_Storage& operator=(T&& o) {
        std::destroy_at(std::addressof(value));
        new (&value) T{ std::move(o) };
        return *this;
    }

    template <typename TT>
    Edge_Storage(const Edge_Storage<TT>& o) : value{ o.value } {}
    template <typename TT>
    Edge_Storage(Edge_Storage<TT>&& o) : value{ std::move(o.value) } {}
    template <typename TT>
    Edge_Storage& operator=(const Edge_Storage<TT>& o) {
        value = o.value;
        return *this;
    }
    template <typename TT>
    Edge_Storage& operator=(Edge_Storage<TT>&& o) {
        value = std::move(o.value);
        return *this;
    }

    const T& operator*() const { return value; }
    const T& get() const { return value; }

private:
    template <typename, typename>
    friend class Edge_Storage;
    T value;
};

template <typename T>
class Edge_Storage<T, std::enable_if_t<std::is_reference_v<T>, void>> {
public:
    Edge_Storage(std::add_const_t<T> o) : value{ std::addressof(o) } {}
    Edge_Storage& operator=(std::add_const_t<T> o) {
        value = std::addressof(o);
        return *this;
    }

    // these can be way better but let's not do anything more with it now
    Edge_Storage(const Edge_Storage& o) : value{ o.value } {}
    Edge_Storage& operator=(const Edge_Storage& o) {
        value = o.value;
        return *this;
    }

    const T& operator*() const { return *value; }
    const T& get() const { return *value; }

private:
    std::add_pointer_t<std::remove_reference_t<T>> value;
};
} // namespace detail

template <typename T>
class Edge {
public:
    // @TODO(Marcus): we probably need to check for constness and all.
    using value_type       = T;
    using reference        = std::remove_reference_t<T>&;
    using const_reference  = const std::decay_t<T>&;
    using rvalue_reference = std::decay_t<T>&&;

    operator const_reference() const { return *value; }
    const_reference get() const { return *value; }
    const_reference operator*() const { return get(); }

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
    Edge(const_reference o) : value{ o } {
        // don't need to broadcast this since this is initial value
        // broadcast();
    }

    Edge(rvalue_reference o) : value{ std::move(o) } {
        // don't need to broadcast this since this is initial value
        // broadcast();
    }
    Edge& operator=(const_reference o) {
        value = o;
        broadcast();
        return *this;
    }
    Edge& operator=(rvalue_reference o) {
        value = std::move(o);
        broadcast();
        return *this;
    }

    void set(const_reference o) {
        value = o;
        broadcast();
    }

    void set(rvalue_reference o) {
        value = std::move(o);
        broadcast();
    }

    void on_changed(mini::Delegate<void()> delegate) { on_changed_listeners.emplace_back(std::move(delegate)); }

    void broadcast() {
        for (auto& on_changed : on_changed_listeners) {
            on_changed();
        }
    }

    void reset() { on_changed_listeners.clear(); }

private:
    template <typename TT>
    friend class Edge;
    detail::Edge_Storage<T> value;

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
        on_changed_fn(+[](void* p, mini::Delegate<void()> delegate) {
            static_cast<Edge<TT>*>(p)->on_changed(std::move(delegate));
        }) {}

    decltype(auto) get() const {
        assert(converter);
        return converter(reference);
    }

    void on_changed(mini::Delegate<void()> delegate) {
        assert(on_changed_fn);
        on_changed_fn(reference, std::move(delegate));
    }

private:
    void* reference;
    using Converter_Function  = T (*)(void*);
    using On_Changed_Function = void (*)(void*, mini::Delegate<void()>);

    Converter_Function converter;
    On_Changed_Function on_changed_fn;
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
