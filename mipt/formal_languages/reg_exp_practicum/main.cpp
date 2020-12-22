#include <iostream>
#include "suffix.h"


int main() {
  std::string reg_exp = "";
  std::cin >> reg_exp;
  char x;
  size_t k;
  std::cin >> x >> k;

  try {
    if (NRegExp::ContainsSuffix(reg_exp, x, k)) {
      std::cout << "YES\n";
    } else {
      std::cout << "NO\n";
    }
  } catch (const NRegExp::TParser::RegExpParserException e) {
    std::cout << "Error while parsing regular expression: "
              << e.what() << std::endl;
  }
  return 0;
}
