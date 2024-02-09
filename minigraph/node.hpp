#pragma once
#include <tuple>

namespace mini {

namespace meta {

template <typename T>
class Node {
public:
private:
    T internal_callable;
};

// make a tuple-like check for the struct
template <typename T, typename I, typename... Args>
decltype(auto) node(I, Args&&... args) {}

} // namespace meta

} // namespace mini
