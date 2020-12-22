#ifndef TSP_PLANEGRAPH
#define TSP_PLANEGRAPH

#include <cassert>
#include <memory>
#include <optional>
#include <set>
#include <vector>
#include "Graph.h"
#include "Point.h"

template <class T = long double>
class PlaneGraph {
 public:
  explicit PlaneGraph(size_t size_init = 0) {
    vertex_count_ = size_init;
    vertices_.resize(size_init);
  }

  size_t GetSize() const { return vertex_count_; }

  void AddVertex(T x, T y) {
    vertices_.push_back(Point<T>(x, y));
    ++vertex_count_;
  }

  long double GetDistances(size_t first, size_t second) {
    assert(first < GetSize());
    assert(second < GetSize());

    return GetLength(vertices_[first], vertices_[second]);
  }

  std::shared_ptr<Graph<T>> GetMst();

  long double FindPreciseHamiltonianCycle();

  long double FindRoughHamiltonianPath();

 private:
  size_t vertex_count_;
  std::vector<Point<T>> vertices_;

  std::optional<long double> Min(std::optional<long double> &first,
                                 long double second) {
    return std::make_optional(first ? std::min(first.value(), second) : second);
  }
};

#endif  // TSP_PLANEGRAPH
