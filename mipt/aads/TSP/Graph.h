#ifndef TSP_GRAPH_H
#define TSP_GRAPH_H

#include <stack>
#include <unordered_map>
#include <vector>

template <class T = long double>
class Graph {
 public:
  explicit Graph(size_t size_init) : vertex_count_(size_init) { adjacency_list_.resize(size_init); }

  size_t GetSize() const { return vertex_count_; }

  void AddEdge(size_t from, size_t to, T weight) {
    assert(from < GetSize());
    assert(to < GetSize());

    adjacency_list_[from][to] = weight;
    adjacency_list_[to][from] = weight;
  }

  void GetEulerCycle(std::vector<size_t>& ordering, size_t start = 0) {
    ordering.clear();
    std::vector<std::vector<size_t>> adjacency_list(GetSize(), std::vector<size_t>());
    for (size_t from = 0; from < GetSize(); ++from) {
      for (auto& [to, weight] : adjacency_list_[from]) {
        adjacency_list[from].push_back(to);
      }
    }

    std::stack<size_t> vertices;
    vertices.push(start);

    while (!vertices.empty()) {
      size_t vertex = vertices.top();

      if (adjacency_list[vertex].empty()) {
        ordering.push_back(vertex);
        vertices.pop();
      } else {
        size_t adjacent = adjacency_list[vertex].back();
        adjacency_list[vertex].erase(adjacency_list[vertex].end() - 1);
        vertices.push(adjacent);
      }
    }
  }

 private:
  size_t vertex_count_;
  std::vector<std::unordered_map<size_t, T>> adjacency_list_;
};

#endif  // TSP_GRAPH_H
