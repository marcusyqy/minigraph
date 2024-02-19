#include "minigraph/function_ref.hpp"
#include <functional>
#include <iostream>
#include <utility>

class Class {
public:
    explicit Class(int _1, int _2) : x(_1), y(_2) {}

    Class(const Class& o)            = delete;
    Class& operator=(const Class& o) = delete;

    Class(Class&& o)            = delete;
    Class& operator=(Class&& o) = delete;

    void print() const {
        std::cout << "Print called from Class" << std::endl;
        std::cout << "x : " << x << std::endl;
        std::cout << "y : " << y << std::endl;
    }

    int get_x() const { return x; }
    int get_y() const { return y; }

private:
    int x;
    int y;
};

class Function {
public:
    int operator()(Class&& c) const {
        std::cout << name << " const was called!" << std::endl;
        c.print();
        return c.get_x() + c.get_y();
    }

    double operator()(Class&& c) {
        std::cout << name << " was called!" << std::endl;
        c.print();
        return static_cast<double>(c.get_x() + c.get_y());
    }

    Function(const Function& o) { *this = o; }

    Function& operator=(const Function& o) {
        std::cout << "name was copied : " << o.name << std::endl;
        name = o.name;
        return *this;
    }

    Function(std::string o) : name(std::move(o)) {}

private:
    std::string name;
};

int main(int argc, char** argv) {
    auto c             = Class(4, 2);
    const auto inplace = Function("inplace");
    mini::Function_Ref<int(Class &&)> fn_ref_inplace{ inplace };
    std::cout << "result from " << fn_ref_inplace(std::move(c)) << std::endl;

    auto copied = Function("copied");
    mini::Function_Ref<int(Class &&)> fn_ref_copied{ copied };
    std::cout << "result from " << fn_ref_copied(Class(3, 1)) << std::endl;

    auto ref = Function("ref");
    mini::Function_Ref<double(Class &&)> fn_ref_ref{ ref };
    std::cout << "result from " << fn_ref_ref(std::move(c)) << std::endl;
}