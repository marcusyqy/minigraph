#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include <cassert>
#include <iostream>

struct Add {
    double operator()(double x, double y) const { return x + y; }
};

struct Multiply {
    double operator()(double x, double y) const { return x * y; }
};

int main(int argc, char** argv) {
    using namespace mini;
    // stable API so we don't need C++17
    auto x    = edge(0.0);
    double aa = 1.0;
    auto y    = edge(aa);

    auto add_node     = node({ x, y }, initialize<Add>(, { 1.0 }));
    auto [add_result] = add_node.edges();

    auto z        = edge<double>(0.0);
    auto mul_node = node<Multiply>({ z, add_result });

    std::cout << "hello" << std::endl;

    return 0;
}