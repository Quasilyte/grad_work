#pragma once

#include "typedefs.hpp"
#include "dt/str_view.hpp"
#include <vector>

namespace dt {
  template<class T> class Hlist;
}

template<class T> class dt::Hlist {
public:
  struct Node {
    u64 hashed_key = 0;
    T val;

    Node() = default;
    Node(const StrView& key, T val);
  };

  void Insert(const StrView& key, T val);

  void Drop(int n);

  T Find(const StrView& key) const noexcept;

private:
  std::vector<Node> nodes;
};