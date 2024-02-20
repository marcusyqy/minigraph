#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include <cassert>
#include <iostream>
#include <thread>

struct Add {
    double operator()(double x, double y) const {
        std::cout << "Add is called" << std::endl;
        return x + y;
    }
};

struct Multiply {
    double operator()(double x, double y) const {
        std::cout << "Multiply is called" << std::endl;
        return x * y;
    }
};

class Executor {
public:
private:
    std::thread execution_thread;
};

int main(int argc, char** argv) {
    using namespace mini;
    Executor react; // @TODO: figure out how to incorporate this

    auto x    = Edge(5.0);
    double aa = 1.0;
    auto y    = Edge(aa);

    auto add_node = Node<Add>({ x, y });

    auto& [add_result] = add_node.edges();

    auto z             = Edge<double>(7.0);
    auto mul_node      = Node<Multiply>({ z, add_result });
    auto& [mul_result] = mul_node.edges();

    x = 4.0; // this update should cause one run.
    y = 8.0; // this update should cause another run.

    // these should be automatic.
    // add_node();
    // mul_node();

    return 0;
}