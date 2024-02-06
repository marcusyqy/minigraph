#include "../minigraph/edge.hpp"
#include <iostream>
#include <cassert>

struct Convertible_To_Int {
    float value;
    operator int() const { return (int)value; } // needed for relaxed_edge
};

struct Not_Convertible_To_Int {
    float value;
};

int main(int argc, char** argv) {
    mini::Edge<Convertible_To_Int> x1{Convertible_To_Int{4.1f}};
    mini::Edge<Not_Convertible_To_Int> x2{Not_Convertible_To_Int{4.1f}};
    mini::Relaxed_Edge<int> y1 {x1};
    // mini::Relaxed_Edge<int> y2 {x2}; (NC)

    std::cout << y1.get() << std::endl;
    assert(y1.get() == 4);
    std::cout << "hello world" << std::endl;

    return 0;
}