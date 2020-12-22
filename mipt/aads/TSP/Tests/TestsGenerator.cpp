#ifndef TSP_TESTSGENERATOR
#define TSP_TESTSGENERATOR

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

template <class T = double>
class TestsGenerator {
 public:
  void Generate(size_t n, size_t test_index = 0, T deviation = 1, T mean = 0) {
    std::ofstream test_file;
    test_file.open("Tests/" + std::to_string(test_index) + ".txt");

    test_file << n << '\n';

    // construct a trivial random generator engine from a time-based seed:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::normal_distribution<T> distribution(mean, deviation);

    for (size_t i = 0; i < n; ++i) {
      test_file << distribution(generator) << ' ' << distribution(generator) << '\n';
    }

    test_file.close();
  }
};

#endif  // TSP_TESTSGENERATOR
