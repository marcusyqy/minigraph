#pragma once
#include "delegate.hpp"
#include "edge.hpp"
#include "meta.hpp"
#include <tuple>
#include <vector>

namespace mini {

namespace meta {
namespace detail {

template <typename>
struct Type_List_To_Tuple_Impl;

// @TODO: support more than just this tuple
template <typename... Ts>
struct Type_List_To_Tuple_Impl<Type_List<Ts...>> {
  using type = std::tuple<Ts...>;
};
template <typename T>
using Type_List_To_Tuple = typename Type_List_To_Tuple_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Relaxed_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Relaxed_Edge_Impl<Type_List<Ts...>> {
  using type = Type_List<Relaxed_Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Relaxed_Edge = typename Decorate_Type_List_With_Relaxed_Edge_Impl<T>::type;

template <typename>
struct Decorate_Type_List_With_Edge_Impl;
template <typename... Ts>
struct Decorate_Type_List_With_Edge_Impl<Type_List<Ts...>> {
  using type = Type_List<Edge<Ts>...>;
};
template <typename T>
using Decorate_Type_List_With_Edge = typename Decorate_Type_List_With_Edge_Impl<T>::type;

template <typename T>
using Params = typename meta::Callable_Traits<T>::param_list;

template <typename T>
using Returns = typename meta::Callable_Traits<T>::return_list;

template <typename T>
using Return_Type = typename meta::Callable_Traits<T>::return_type;

template <typename T>
using Node_Input = Type_List_To_Tuple<Decorate_Type_List_With_Relaxed_Edge<Params<T>>>;

template <typename T>
using Node_Output = Type_List_To_Tuple<Decorate_Type_List_With_Edge<Returns<T>>>;

} // namespace detail
} // namespace meta

template <typename T>
struct Node {
public:
  static constexpr auto nonempty_output = meta::detail::Returns<T>::size != 0;
  using Inputs                          = meta::detail::Node_Input<T>;
  using Outputs           = std::conditional_t<nonempty_output, meta::detail::Node_Output<T>, std::tuple<>>;
  using Ref_Outputs       = std::conditional_t<nonempty_output, meta::detail::Node_Output<T>&, void>;
  using Const_Ref_Outputs = std::conditional_t<nonempty_output, const meta::detail::Node_Output<T>&, void>;
  using Return_Type       = meta::detail::Return_Type<T>;

  template <typename... Args>
  Node(Inputs o, Args&&... args) :
      callable{ std::forward<Args&&>(args)... }, inputs{ o },
      rank{ get_rank(inputs, std::make_index_sequence<meta::detail::Params<T>::size>{}) },
      outputs{ init(
          callable,
          inputs,
          std::make_index_sequence<meta::detail::Params<T>::size>{},
          std::make_index_sequence<meta::detail::Returns<T>::size>{},
          rank) } {
    attach_callback_to_edges(std::make_index_sequence<meta::detail::Params<T>::size>{});
  }

  void operator()() {
    apply(
        std::make_index_sequence<meta::detail::Params<T>::size>{},
        std::make_index_sequence<meta::detail::Returns<T>::size>{});
    out_of_sync = false;
  }

  Ref_Outputs edges() noexcept {
    if constexpr (nonempty_output) {
      return outputs;
    }
  }

  Const_Ref_Outputs edges() const noexcept {
    if constexpr (nonempty_output) {
      return outputs;
    }
  }

  operator bool() const noexcept { return out_of_sync; }
  [[nodiscard]] bool outdated() const noexcept { return out_of_sync; }

  void on_outdated(Delegate<void(Node&)> delegate) { outdated_listeners.emplace_back(std::move(delegate)); }
  uint32_t depth() const { return rank; }

private:
  template <size_t... Is, size_t... Os>
  void apply(std::index_sequence<Is...>, std::index_sequence<Os...>) {
    if constexpr (nonempty_output) {
      // cannot be auto as we need to retain references.
      Return_Type immediate = callable(std::get<Is>(inputs).get()...);
      if constexpr (meta::is_tuple_like<meta::detail::Return_Type<T>>) {
        (..., static_cast<void>(std::get<Os>(outputs) = std::move(std::get<Os>(immediate))));
      } else {
        std::get<0>(outputs) = std::move(immediate);
      }
    } else {
      callable(std::get<Is>(inputs).get()...);
    }
  }

  template <size_t... Is, size_t... Os>
  static Outputs
      init(T& callable, Inputs& inputs, std::index_sequence<Is...>, std::index_sequence<Os...>, uint32_t rank) {
    if constexpr (!nonempty_output) {
      return {};
    } else if constexpr (meta::is_tuple_like<meta::detail::Return_Type<T>>) {
      // cannot be auto as we need to retain references.
      Return_Type immediate = std::invoke(callable, std::get<Is>(inputs).get()...);
      return { { mini::detail::Depth{ rank + 1 }, std::get<Os>(immediate) }... };
    } else {
      return { { mini::detail::Depth{ rank + 1 }, std::invoke(callable, std::get<Is>(inputs).get()...) } };
    }
  }

  template <size_t N>
  void callback() {
    if (!out_of_sync) {
      out_of_sync = true;
      for (auto& on_outdated : outdated_listeners) {
        on_outdated(*this);
      }
    }
    // should we also mark the others as dirty here?
  }

  template <size_t... Is>
  static uint32_t get_rank(const Inputs& inputs, std::index_sequence<Is...>) {
    uint32_t rank = {};
    (..., [&]() {
      auto& input = std::get<Is>(inputs);
      rank        = std::max(rank, input.depth());
    }());
    return rank;
  }

  template <size_t... Is>
  void attach_callback_to_edges(std::index_sequence<Is...>) {
    (..., [&]() {
      auto& input = std::get<Is>(inputs);
      input.on_changed({ connect<&Node::callback<Is>>, *this });
    }());
  }

private:
  T callable;
  Inputs inputs;
  uint32_t rank = {};

  Outputs outputs;
  std::vector<Delegate<void(Node&)>> outdated_listeners;

  bool out_of_sync = false;
};

} // namespace mini
