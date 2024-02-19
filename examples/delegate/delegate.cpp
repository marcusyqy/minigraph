#include "minigraph/delegate.hpp"
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
        std::cout << "operator() const  for " << name << " const was called!" << std::endl;
        c.print();
        return c.get_x() + c.get_y();
    }

    double operator()(Class&& c) {
        std::cout << "operator() for " << name << " was called!" << std::endl;
        c.print();
        return static_cast<double>(c.get_x() + c.get_y());
    }

    int foo(Class&& c) const {
        std::cout << "foo const  for " << name << " was called from foo!" << std::endl;
        c.print();
        return 2 * c.get_x() + c.get_y();
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
    auto c = Class(1, 4);

    auto default_round_bracket = Function("default operator()");
    mini::Delegate<int(Class &&)> fn_ref_default{ default_round_bracket };
    std::cout << "result from " << fn_ref_default(Class(3, 1)) << std::endl;

    auto connect_in_place = Function("connect_in_place");
    mini::Delegate<int(Class &&)> fn_ref_inplace{ mini::connect<&Function::foo>, connect_in_place };
    std::cout << "result from " << fn_ref_inplace(std::move(c)) << std::endl;

    auto ref = Function("overload");
    mini::Delegate<double(Class &&)> fn_ref_ref{ ref };
    std::cout << "result from " << fn_ref_ref(std::move(c)) << std::endl;

    fn_ref_ref.connect<&Function::foo>(connect_in_place);
    std::cout << "result from after connect " << fn_ref_ref(std::move(c)) << std::endl;

    fn_ref_ref.connect(default_round_bracket); // notice the return type changes from double to int not by conversion.
    std::cout << "result from after connect " << fn_ref_ref(std::move(c)) << std::endl;
}