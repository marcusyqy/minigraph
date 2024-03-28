#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include <iostream>
#include <string>

namespace my {

struct Result {
  double x;
  Result(double y) { x = y; }
  Result& operator=(const Result& o) = delete;
  // Result(const Result& o) = delete;
};

struct Upgrade {
  std::tuple<Result, Result> operator()(double x, double y) const { return std::make_tuple(Result{ x }, Result{ y }); }
};

struct Switcher {
  const Result& operator()(bool toggle, const Result& x, const Result& y) const {
    if (toggle) return x;
    return y;
  }
};

struct Add {
  double operator()(double x, double y) const { return x + y; }
};

struct Multiply {
  double operator()(double x, double y) const { return x * y; }
};

class Immediate_Executor {
public:
  template <typename T>
  void attach_callback(mini::Node<T>& node) {
    node.on_outdated({ mini::connect<&Immediate_Executor::callback<T>>, *this });
  }

private:
  template <typename T>
  void callback(mini::Node<T>& node) {
    node();
  }
};

} // namespace my

int main() {
  my::Immediate_Executor immediate_executor;
  // 1
  auto x1 = mini::Edge(5.0);
  auto y1 = mini::Edge(7.0);
  auto z1 = mini::Edge(8.0);

  auto add1 = mini::Node<my::Add>({ x1, y1 });
  immediate_executor.attach_callback(add1);
  auto& [add1_result] = add1.edges();

  auto mul1 = mini::Node<my::Multiply>({ z1, add1_result });
  immediate_executor.attach_callback(mul1);
  auto& [mul1_result] = mul1.edges();

  // 2
  auto x2 = mini::Edge(8.0);
  auto y2 = mini::Edge(4.0);
  auto z2 = mini::Edge(18.0);

  auto add2 = mini::Node<my::Add>({ x2, y2 });
  immediate_executor.attach_callback(add2);
  auto& [add2_result] = add2.edges();

  auto mul2 = mini::Node<my::Multiply>({ z2, add2_result });
  immediate_executor.attach_callback(mul2);
  auto& [mul2_result] = mul2.edges();

  auto upgrade = mini::Node<my::Upgrade>({ mul1_result, mul2_result });
  immediate_executor.attach_callback(upgrade);
  auto& [upgrade1, upgrade2] = upgrade.edges();

  auto condition = mini::Edge(false);
  auto switcher  = mini::Node<my::Switcher>({ condition, upgrade1, upgrade2 });
  immediate_executor.attach_callback(switcher);
  auto& [result] = switcher.edges();

  std::cout << "=============================================================" << std::endl;
  std::cout << "begin(1)" << std::endl;
  std::cout << "x : " << x1.get() << std::endl;
  std::cout << "y : " << y1.get() << std::endl;
  std::cout << "z : " << z1.get() << std::endl;
  std::cout << "x+y : " << add1_result.get() << std::endl;
  std::cout << "(x+y)*z : " << mul1_result.get() << std::endl;
  std::cout << "result : " << upgrade1.get().x << std::endl;
  std::cout << "=============================================================" << std::endl;
  std::cout << "begin(2)" << std::endl;
  std::cout << "x : " << x2.get() << std::endl;
  std::cout << "y : " << y2.get() << std::endl;
  std::cout << "z : " << z2.get() << std::endl;
  std::cout << "x+y : " << add2_result.get() << std::endl;
  std::cout << "(x+y)*z : " << mul2_result.get() << std::endl;
  std::cout << "result : " << upgrade2.get().x << std::endl;
  std::cout << std::endl;

  auto print = [&] {
    std::cout << "=============================================================" << std::endl;
    std::cout << "switcher" << std::endl;
    std::cout << "switcher result: " << result.get().x << std::endl;
    std::cout << "switcher condition: " << std::boolalpha << condition.get() << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << std::endl;
  };

  condition = true;
  print();
  condition = false;
  print();
  condition = true;
  print();
}