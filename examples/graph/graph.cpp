#include "minigraph/edge.hpp"
#include "minigraph/mem.hpp"
#include "minigraph/node.hpp"
#include <iostream>

struct Resolver {
  void resolve() {
    std::cout << "Run" << std::endl;
    while (head) {
      head->delegate();
      head = head->next;
    }
    std::cout << "Run End" << std::endl;
    clear();
  }

  void clear() {
    head = nullptr;
    tail = nullptr;
    arena.clear();
  }

  template <typename T>
  void attach(mini::Node<T>& node) {
    node.on_outdated({ mini::connect<&Resolver::callback<T>>, *this });
  }

private:
  template <typename T>
  void callback(mini::Node<T>& node) {
    auto n = arena.push<Node>(node, nullptr);
    if (tail) {
      tail->next = n;
      tail       = tail->next;
    } else {
      head = n;
      tail = n;
    }
    std::cout << "Node depth is: " << std::dec << node.depth() << std::endl;
  }

private:
  mini::Default_Stack_Allocator arena;
  struct Node {
    mini::Delegate<void()> delegate;
    Node* next;
  } *head = nullptr, *tail = nullptr;
};

struct Graph {
  template <typename T>
  mini::Edge<T>& edge(T&& t) {
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << ", align " << alignof(mini::Edge<T>)
              << std::endl;
    auto e = arena.push<mini::Edge<T>>(t);
    return *e;
  }

  template <typename T>
  mini::Edge<T>& edge(const T& t) {
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << ", align " << alignof(mini::Edge<T>)
              << std::endl;
    auto e = arena.push<mini::Edge<T>>(t);
    return *e;
  }

  template <typename T, typename... Args>
  typename mini::Node<T>::Ref_Outputs node(typename mini::Node<T>::Inputs inputs, Args&&... args) {
    std::cout << "allocating node:" << std::dec << sizeof(mini::Node<T>) << ", align " << alignof(mini::Node<T>)
              << std::endl;
    auto node = arena.push<mini::Node<T>>( inputs, std::forward<Args&&>(args)... );
    resolver.attach(*node);
    return node->edges();
  }

  void run() { resolver.resolve(); }

  void clear() {
    resolver.clear();
    arena.clear();
  }

  ~Graph() { clear(); }

private:
  mini::Default_Stack_Allocator arena;
  Resolver resolver;
};

// Nodes.
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

struct Result {
  double x;
  Result(double y) { x = y; }
  Result& operator=(const Result& o) = delete;
  ~Result() { std::cout << "Destructing Result" << std::endl; }
};

struct Upgrade {
  std::tuple<Result, Result> operator()(double x, double y) const { return std::make_tuple(Result{ x }, Result{ y }); }
};

struct Switcher {
  const Result& operator()(bool toggle, const Result& x, const Result& y) const {
    std::cout << "Switcher operator(): " << toggle << ", " << x.x << ", " << y.x << std::endl;
    if (toggle) return x;
    return y;
  }
  ~Switcher() { std::cout << "Destructing Switcher" << std::endl; }
};

struct Printer {
  void operator()(const Result& result) const { std::cout << "From Printer :" << result.x << std::endl; }
};

int main() {
  std::cout << "Size of delegate:" << sizeof(std::vector<mini::Delegate<void()>>) << std::endl;
  Graph graph{};
  // 1
  auto x1 = graph.edge(5.0);
  auto y1 = graph.edge(7.0);
  auto z1 = graph.edge(8.0);

  auto& [add1_result] = graph.node<Add>({ x1, y1 });
  auto& [mul1_result] = graph.node<Multiply>({ z1, add1_result });

  // 2
  auto x2 = graph.edge(8.0);
  auto y2 = graph.edge(4.0);
  auto z2 = graph.edge(18.0);

  auto& [add2_result] = graph.node<Add>({ x2, y2 });

  auto& [mul2_result] = graph.node<Multiply>({ z2, add2_result });

  auto& [upgrade1, upgrade2] = graph.node<Upgrade>({ mul1_result, mul2_result });

  auto condition = graph.edge(false);
  auto& [result] = graph.node<Switcher>({ condition, upgrade1, upgrade2 });
  graph.node<Printer>({ result });

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

  std::cout << "=============================================================" << std::endl;
  std::cout << "switcher" << std::endl;
  std::cout << "switcher result: " << std::hex << result.get().x << std::endl;
  std::cout << "switcher condition: " << std::boolalpha << condition.get() << std::endl;
  std::cout << "=============================================================" << std::endl;
  std::cout << std::endl;

  auto print = [&] {
    graph.run();
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

  x1 = 10;
  graph.run();
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
}
