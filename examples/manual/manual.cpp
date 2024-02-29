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

int main() {
    using namespace mini;
    // stable API so we don't need C++17
    auto x    = Edge(5.0);
    double aa = 1.0;
    auto y    = Edge(aa);

    auto add_node      = Node<Add>({ x, y });
    auto& [add_result] = add_node.edges();

    auto z             = Edge<double>(7.0);
    auto mul_node      = Node<Multiply>({ z, add_result });
    auto& [mul_result] = mul_node.edges();

    std::cout << "before add" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    x = 4.0;
    y = 8.0;

    std::cout << "before add change to x and y" << std::endl;
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