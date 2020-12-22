#include "PlaneGraph.h"

// Using dynamic programming, O(V^2 * 2^V)
template <class T>
long double PlaneGraph<T>::FindPreciseHamiltonianCycle() {
  std::vector<std::vector<std::optional<long double>>> path_weight(
      1 << GetSize(),
      std::vector<std::optional<long double>>(GetSize(), std::nullopt));
  size_t start = 0;

  path_weight[1 << start][start] = 0;
  for (size_t mask = 1; mask < (1 << GetSize()); ++mask) {
    for (size_t i = 1; i < GetSize(); ++i) {
      if ((mask >> i & 1) == 1 && ((mask >> start) & 1) == 1) {
        size_t new_mask = mask ^ (1 << i);

        for (size_t j = 0; j < GetSize(); ++j) {
          if ((new_mask >> j & 1) == 1 && path_weight[new_mask][j]) {
            path_weight[mask][i] =
                Min(path_weight[mask][i],
                    path_weight[new_mask][j].value() + GetDistances(i, j));
          }
        }
      }
    }
  }

  std::optional<long double> length = std::nullopt;
  for (size_t i = 1; i < GetSize(); ++i) {
    length = Min(length, path_weight[(1 << GetSize()) - 1][i].value() +
                             GetDistances(i, start));
  }

  return length.value();
}

// Prim's algorithm, O(V^2)
template <class T>
std::shared_ptr<Graph<T>> PlaneGraph<T>::GetMst() {
  std::vector<std::pair<long double, size_t>> distances(GetSize());
  std::vector<bool> processed(GetSize(), false);

  // Initializing
  size_t start = 0;
  processed[start] = true;
  for (size_t vertex = 0; vertex < GetSize(); ++vertex) {
    distances[vertex] = {GetDistances(start, vertex), start};
  }

  std::shared_ptr<Graph<T>> mst = std::make_shared<Graph<T>>(Graph<T>(GetSize()));

  for (size_t i = 1; i < GetSize(); ++i) {
    std::optional<size_t> best_vertex = std::nullopt;
    for (size_t vertex = 0; vertex < GetSize(); ++vertex) {
      if (!processed[vertex] &&
          (!best_vertex || distances[best_vertex.value()].first > distances[vertex].first)) {
        best_vertex = vertex;
      }
    }

    mst->AddEdge(distances[best_vertex.value()].second, best_vertex.value(),
                 distances[best_vertex.value()].first);

    for (size_t to = 0; to < GetSize(); ++to) {
      if (!processed[to] &&
          distances[to].first > GetDistances(best_vertex.value(), to)) {
        distances[to] = {GetDistances(best_vertex.value(), to), best_vertex.value()};
      }
    }
    processed[best_vertex.value()] = true;
  }

  return mst;
}

// Ordinary mst approximation (2), O(V^2)
template <class T>
long double PlaneGraph<T>::FindRoughHamiltonianPath() {
  if (GetSize() < 2) {
    return 0;
  }
  std::shared_ptr<Graph<T>> mst = GetMst();
  std::vector<size_t> cycle(0);
  mst->GetEulerCycle(cycle);

  long double length = 0;
  std::set<size_t> counted_vertices;
  size_t previous_vertex = cycle[0];
  for (size_t vertex : cycle) {
    if (counted_vertices.find(vertex) == counted_vertices.end()) {
      length += GetDistances(previous_vertex, vertex);
      previous_vertex = vertex;
      counted_vertices.insert(vertex);
    }
  }

  length += GetDistances(previous_vertex, cycle[0]);

  return length;
}
