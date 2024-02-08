#include "../minigraph/edge.hpp"
#include "../minigraph/node.hpp"
#include <iostream>
#include <cassert>

struct Add {
    double operator()(double x, double y) const noexcept {
        return x + y;
    }
};

struct Multiply {
    double operator()(double x, double y) const noexcept {
        return x * y;
    }
};

int main(int argc, char** argv) {
    using namespace mini;
    // only for c++17
#if __cplusplus >= 201703L // if more than c++17
    auto x = edge(0.0);
    auto y = edge(0.0);
    auto z = edge(0.0);
    auto n = edge<double>(0.0);

    double aa = 1.0;
    auto i = edge<double>(aa);
    auto m = edge<double>(aa);

    // auto [add_result] = node<Add>(x, y);
    // auto [mul_result] = node<Multiply>(z, add_result);

#endif
    std::cout << "hello" << std::endl;
    
    return 0;
}