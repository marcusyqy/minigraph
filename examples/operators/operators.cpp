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
#if operators_defined
    using namespace mini;
    auto x = edge(5.0);
    auto y = edge(1.0);

    auto add_result = x + y;
    auto z          = edge(7.0);
    auto mul_result = z * add_result;

    std::cout << "before add" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    std::cout << "after add:before mul" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    std::cout << "after mul" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;

    return 0;
#endif // operators_defined
}