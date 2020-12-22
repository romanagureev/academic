#include <iomanip>
#include <iostream>
#include "PlaneGraph.cpp"
#include "Tests/TestsGenerator.cpp"

const size_t quantity = 15;
const size_t max_size = 22;
const size_t first_column = 6;
const size_t middle_column = 16;
const size_t last_column = 3;
const size_t precision = 10;

void MakeTest(size_t size) {
  TestsGenerator<double> gen;
  gen.Generate(size, size, 1);
}

std::pair<long double, long double> Calculate(size_t test) {
  PlaneGraph<double> graph(0);

  std::ifstream input;
  input.open("Tests/" + std::to_string(test) + ".txt");

  size_t n = 0;
  input >> n;
  for (size_t i = 0; i < n; ++i) {
    double x = 0;
    double y = 0;
    input >> x >> y;
    graph.AddVertex(x, y);
  }
  input.close();

  return {graph.FindPreciseHamiltonianCycle(),
          graph.FindRoughHamiltonianPath()};
}

int main() {
  std::ofstream result;
  result.open("result.txt");

  result << std::left << std::setw(first_column) << "Size"
                      << std::setw(middle_column) << "Mean precise"
                      << std::setw(middle_column) << "Mean rough (2)"
                      << std::setw(middle_column) << "Deviation"
                      << std::setw(last_column) << "%"<< '\n'
                      << std::setprecision(precision) << std::endl;

  for (size_t size = 2; size <= max_size; ++size) {
    if (size % 10 == 1) { result << '\n'; }

    long double deviation = 0;
    long double mean_precise = 0;
    long double mean_approximate_2 = 0;

    for (size_t test = 2; test <= quantity; ++test) {
      MakeTest(size);
      auto answer = Calculate(size);
      mean_precise += answer.first;
      mean_approximate_2 += answer.second;
      deviation += (answer.first - answer.second) * (answer.first - answer.second);
    }

    mean_precise /= quantity;
    mean_approximate_2 /= quantity;
    deviation = sqrt(deviation / quantity);

    result << std::left << std::setw(first_column) << size
                        << std::setw(middle_column) << mean_precise
                        << std::setw(middle_column) << mean_approximate_2
                        << std::setw(middle_column) << deviation
                        << std::setw(last_column) << round((mean_approximate_2 - mean_precise) /
                                                           mean_precise * 100)
                        << std::endl;

    std::cout << "size " << size << " processed\n";
  }
  result << '\n' << "Quantity of tests: " << quantity << std::endl;
  result.close();
  return 0;
}
