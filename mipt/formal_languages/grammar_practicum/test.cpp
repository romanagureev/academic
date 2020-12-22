#include <fstream>
#include <iostream>
#include <istream>
#include <vector>

#include "CFGRecognizer.h"

std::vector<std::string> grammar_files = {"tests", "basic"};
std::vector<std::string> test_filenames = {"small", "medium", "big"};

// Returns quantity of passed tests
template <class CharT = char>
size_t Test(std::vector<NCFGrammar::TRecognizer<CharT>>& recognizers,
            const std::basic_string<CharT>& word,
            const std::map<std::string, bool>& verdicts) {
  size_t passed = 0;
  for (auto& recognizer : recognizers) {
    auto it = verdicts.find(recognizer.GetName());
    if (it != verdicts.end()) {
      if (recognizer.Check(word) != it->second) {
        std::cerr << "TEST " << recognizer.GetName() << " on " << word << " failed.\n";
      } else {
        ++passed;
      }
    }
  }

  return passed;
}

template <class CharT = char>
struct TTest {
  std::map<std::string, bool> verdicts;
  std::basic_string<CharT> word;
};

template <class CharT = char>
size_t ParseTests(std::basic_istream<CharT>& in_stream, std::vector<TTest<CharT>>& tests) {
  size_t test_count = 0;

  std::basic_string<CharT> word;
  while (in_stream >> word) {
    TTest<CharT> test;
    test.word = word;

    std::basic_string<CharT> grammar;
    CharT verdict;
    while ((in_stream >> grammar) && (grammar != "end")) {
      in_stream >> verdict;
      test.verdicts[grammar] = (verdict == 'Y') ? true : false;
    }

    tests.push_back(test);

    test_count += test.verdicts.size();
  }

  return test_count;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Expected 1 argument.\n";
    return 1;
  }

  
  std::vector<NCFGrammar::TRecognizer<char>> recognizers;
  for (auto& grammar_file: grammar_files) {
    std::ifstream file("grammars/" + grammar_file);
    while (!file.eof()) {
      recognizers.emplace_back();
      recognizers.back().ParseRules(file);
    }

    file.close();
  }


  for (uint32_t i = 0; i < 3; ++i) {
    if (argv[1][i] == '+') {
      size_t passed = 0, test_count = 0;

      std::ifstream test_file("tests/" + test_filenames[i]);
      std::vector<TTest<char>> tests;
      test_count += ParseTests(test_file, tests);

      for (auto& test: tests) {
        passed += Test(recognizers, test.word, test.verdicts);
      }

      test_file.close();

      std::cerr << test_filenames[i] << ": "
                << passed << '/' << test_count << " passed.\n";
    } else {
      break;
    }
  }

  return 0;
}
