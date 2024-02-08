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

    // only for c++17
    if (__cplusplus == 202101L) std::cout << "C++23";
    else if (__cplusplus == 202002L) std::cout << "C++20";
    else if (__cplusplus == 201703L) std::cout << "C++17";
    else if (__cplusplus == 201402L) std::cout << "C++14";
    else if (__cplusplus == 201103L) std::cout << "C++11";
    else if (__cplusplus == 199711L) std::cout << "C++98";
    else std::cout << "pre-standard C++." << __cplusplus;
    std::cout << "\n";

    auto e = mini::Edge(1);
    std::cout << std::boolalpha << std::is_same_v<typename decltype(e)::value_type, int> << std::endl;
    return 0;
}