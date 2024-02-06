#include "../minigraph/edge.hpp"
#include <iostream>
#include <cassert>

struct Convertible_To_Int {
    int value;
    operator int() const { return value; }
};


int main(int argc, char** argv) {
    mini::Edge<Convertible_To_Int> x{Convertible_To_Int{4}};
    mini::Relaxed_Edge<int> y {x};
    std::cout << y.get() << std::endl;
    assert(y.get() == 4);
    std::cout << "hello world" << std::endl;
    return 0;
}