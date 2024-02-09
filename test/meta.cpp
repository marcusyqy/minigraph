#include "../minigraph/meta.hpp"
#include <iostream>

struct C1 {
    void operator()() {}
};

struct C2 {
    void operator()() noexcept {}
};

struct C3 {
    void operator()() const {}
};

struct C4 {
    void operator()() const noexcept {}
};

void foo(int) {}

int main(int argc, char** argv) {
    std::cout << "tests starting" << std::endl;
    auto xx = mini::meta::Callable_Traits<decltype(&foo)>{};
    auto x1 = mini::meta::Callable_Traits<C1>{};
    auto x2 = mini::meta::Callable_Traits<C2>{};
    auto x3 = mini::meta::Callable_Traits<C3>{};
    auto x4 = mini::meta::Callable_Traits<C4>{};
    std::cout << std::boolalpha
              << std::is_same_v<typename decltype(xx)::param_list, typename decltype(x1)::param_list> << std::endl;
    std::cout << std::boolalpha
              << std::is_same_v<typename decltype(x1)::param_list, typename decltype(x2)::param_list> << std::endl;
    std::cout << std::boolalpha
              << std::is_same_v<typename decltype(x2)::param_list, typename decltype(x3)::param_list> << std::endl;
    std::cout << std::boolalpha
              << std::is_same_v<typename decltype(x3)::param_list, typename decltype(x4)::param_list> << std::endl;
    std::cout << "tests ended" << std::endl;
}