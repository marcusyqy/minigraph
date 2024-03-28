#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include <cassert>
#include <iostream>
#include <mutex>
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

class Batch_Executor {
public:
  template <typename T>
  void attach_callback(mini::Node<T>& node) {
    node.on_outdated({ mini::connect<&Batch_Executor::callback<T>>, *this });
  }

  // for locking.
  void begin_batch() { blocked = true; }
  void end_batch() {
    blocked = false;
    for (auto& node : nodes) {
      node.second();
    }
  }

private:
  template <typename T>
  void callback(mini::Node<T>& node) {
    if (!blocked) {
      node();
    } else {
      nodes[static_cast<const void*>(&node)] = node;
    }
  }

  bool blocked;
  std::unordered_map<const void*, mini::Delegate<void()>> nodes;
};

int main() {
  using namespace mini;
  Batch_Executor executor; // @TODO: figure out how to incorporate this

  auto x    = Edge(5.0);
  double aa = 1.0;
  auto y    = Edge(aa);

  auto add_node = Node<Add>({ x, y });

  auto& [add_result] = add_node.edges();

  auto z             = Edge<double>(7.0);
  auto mul_node      = Node<Multiply>({ z, add_result });
  auto& [mul_result] = mul_node.edges();

  executor.attach_callback(add_node);
  executor.attach_callback(mul_node);

  auto print_state = [&] {
    std::cout << "=============================================================" << std::endl;
    std::cout << "x : " << x.get() << std::endl;
    std::cout << "y : " << y.get() << std::endl;
    std::cout << "z : " << z.get() << std::endl;
    std::cout << "x+y : " << add_result.get() << std::endl;
    std::cout << "(x+y)*z : " << mul_result.get() << std::endl;
    std::cout << "=============================================================" << std::endl;
  };

  executor.begin_batch();
  std::cout << "before x assignment:" << std::endl;
  print_state();
  x = 4.0; // this update should cause one run.
  std::cout << "after x assignment:" << std::endl;

  std::cout << "before y assignment:" << std::endl;
  print_state();
  y = 8.0; // this update should cause another run.
  executor.end_batch();

  std::cout << "after y assignment:" << std::endl;
  print_state();

  return 0;
}