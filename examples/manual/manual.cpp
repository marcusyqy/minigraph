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
    auto x    = edge(5.0);
    double aa = 1.0;
    auto y    = edge(aa);

    auto add_node      = node<Add>({ x, y });
    auto& [add_result] = add_node.edges();

    auto z             = edge<double>(7.0);
    auto mul_node      = node<Multiply>({ z, add_result });
    auto& [mul_result] = mul_node.edges();

    std::cout << "before add" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    add_node();
    std::cout << "after add:before mul" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    mul_node();
    std::cout << "after mul" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    return 0;
}