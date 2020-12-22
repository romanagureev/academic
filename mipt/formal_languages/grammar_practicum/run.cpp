#include <fstream>
#include <iostream>
#include <istream>
#include <unordered_map>

#include "CFGRecognizer.h"

std::vector<std::string> grammar_files = {"tests", "basic"};

void ReadGrammars(std::unordered_map<std::string, NCFGrammar::TRecognizer<char>>& recognizers) {
  for (auto& grammar_file: grammar_files) {
    std::ifstream file("grammars/" + grammar_file);
    while (!file.eof()) {
      NCFGrammar::TRecognizer<char> recognizer;
      recognizer.ParseRules(file);
      recognizers.insert({recognizer.GetName(), recognizer});
    }

    file.close();
  }
}

int main(int argc, char* argv[]) {
  std::unordered_map<std::string, NCFGrammar::TRecognizer<char>> recognizers;
  ReadGrammars(recognizers);

  std::string word;
  while (std::cin >> word) {
    for (size_t i = 1; i < argc; ++i) {
      std::string recognizer_name(argv[i]);
      std::cout << recognizer_name << ": "
                << ((recognizers[recognizer_name].Check(word)) ? 'Y' : 'N') << '\n';
    }
  }

  return 0;
}

