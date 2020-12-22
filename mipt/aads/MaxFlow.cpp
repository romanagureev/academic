/*
 * Задан неориентированный граф, каждое ребро которого обладает целочисленной
 * пропускной способностью. Найдите максимальный поток из вершины с номером 1
 * в вершину с номером n.
 * Алгоритм Эдмондса-Карпа O(V * E^2)
 */
#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

class Graph {
 public:
  explicit Graph(size_t size_init)
      : vertex_count_(size_init), adjacency_list_{size_init} {}

  explicit Graph(Graph* graph_init)
      : vertex_count_(graph_init->GetSize()),
        adjacency_list_(graph_init->adjacency_list_) {}

  size_t GetSize() const { return vertex_count_; }

  void ReadFromStdIn(size_t edge_count) {
    size_t from = 0;
    size_t to = 0;
    size_t weight = 0;
    for (size_t i = 0; i < edge_count; ++i) {
      std::cin >> from >> to >> weight;

      assert(from - 1 < GetSize());
      assert(to - 1 < GetSize());
      assert(weight <= 100 * 1000);

      adjacency_list_[from - 1][to - 1] += weight;
    }
  }

  size_t GetWeightOfEdge(size_t from, size_t to) const {
    assert(from < GetSize());
    assert(to < GetSize());

    auto weight_it = adjacency_list_[from].find(to);
    size_t weight = 0;
    if (weight_it != adjacency_list_[from].end()) {
      weight = weight_it->second;
    }

    return weight;
  }

  void AddWeightToEdge(size_t from, size_t to, size_t weight) {
    assert(from < GetSize());
    assert(to < GetSize());

    adjacency_list_[from][to] += weight;
  }

  // Fills 'path' with any shortest path of positive edges from source to sink
  // If no path found returns empty vector
  void FindAnyShortestPath(size_t source, size_t sink,
                           std::vector<size_t>& path) {
    assert(source < GetSize());
    assert(sink < GetSize());

    std::vector<std::optional<size_t>> parent(GetSize(), std::nullopt);
    std::queue<size_t> vertices;
    vertices.push(source);

    // BFS
    while (!parent[sink] && !vertices.empty()) {
      size_t vertex = vertices.front();
      vertices.pop();

      for (auto next : adjacency_list_[vertex]) {
        if (!parent[next.first] && next.second > 0) {
          parent[next.first] = vertex;
          vertices.push(next.first);
        }
      }
    }

    // Recovering the path
    path.clear();
    if (parent[sink]) {
      size_t vertex = sink;
      while (vertex != source) {
        path.push_back(vertex);
        vertex = parent[vertex].value();
      }
      path.push_back(source);
      reverse(path.begin(), path.end());
    }
  }

 private:
  size_t vertex_count_;
  std::vector<std::unordered_map<size_t, size_t>> adjacency_list_;
};

// Edmonds–Karp algorithm, O(V * E^2)
size_t FindMaxFlow(Graph* graph, size_t source, size_t sink) {
  assert(source < graph->GetSize());
  assert(sink < graph->GetSize());

  size_t max_flow = 0;
  Graph residual_network(graph);

  std::vector<size_t> augmenting_path;
  residual_network.FindAnyShortestPath(source, sink, augmenting_path);

  while (!augmenting_path.empty()) {
    size_t path_flow = 0;

    for (int i = 0; i < augmenting_path.size() - 1; ++i) {
      size_t edge_weight = residual_network.GetWeightOfEdge(
          augmenting_path[i], augmenting_path[i + 1]);

      if (path_flow == 0 || path_flow > edge_weight) {
        path_flow = edge_weight;
      }
    }

    max_flow += path_flow;

    for (int i = 0; i < augmenting_path.size() - 1; ++i) {
      size_t from = augmenting_path[i];
      size_t to = augmenting_path[i + 1];
      residual_network.AddWeightToEdge(from, to, -path_flow);
      residual_network.AddWeightToEdge(to, from, path_flow);
    }

    residual_network.FindAnyShortestPath(source, sink, augmenting_path);
  }

  return max_flow;
}

int main() {
  size_t vertex_count = 0;
  size_t edge_count = 0;

  std::cin >> vertex_count >> edge_count;
  assert(vertex_count <= 100);
  assert(edge_count <= 1000);

  Graph graph(vertex_count);
  graph.ReadFromStdIn(edge_count);

  std::cout << FindMaxFlow(&graph, 0, vertex_count - 1);
  return 0;
}

