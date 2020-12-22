#include <exception>
#include <iostream>
#include <stack>
#include "regexp.h"

namespace NRegExp {

class TParser {
public:
  class RegExpParserException : public std::exception {
  public:
    explicit RegExpParserException(std::string message)
      : _message(std::move(message)) {};

    [[nodiscard]] const char* what() const noexcept override {
      return _message.c_str();
    }
  private:
    std::string _message;
  };

  template <class TRegExp>
  static TRegExp Parse(const std::string& reg_exp) {
    std::stack<TRegExp> arguments;

    for (auto symbol: reg_exp) {
      SymbolType symbol_type = Check(symbol);

      if (symbol_type == SymbolType::Skip) {
        continue;
      }

      if (symbol_type == SymbolType::EmptyString) {
        arguments.push(TRegExp());
      } else if (symbol_type == SymbolType::Literal) {
        arguments.push(TRegExp(symbol));
      } else if (symbol_type == SymbolType::Alternation) {
        auto [first_re, second_re] = GetTwoArguments(arguments);
        arguments.push(first_re | second_re);
      } else if (symbol_type == SymbolType::Concatenation) {
        auto [first_re, second_re] = GetTwoArguments(arguments);
        arguments.push(first_re & second_re);
      } else if (symbol_type == SymbolType::KleeneStar) {
        TRegExp re = GetArgument(arguments);
        arguments.push(re.KleeneStar());
      }
    }

    if (arguments.size() > 1) {
      throw RegExpParserException("Not enough operations.");
    }

    if (arguments.empty()) {
      return TRegExp();
    } else {
      return arguments.top();
    }
  }
private:
  enum class SymbolType {
    EmptyString,
    Literal,
    Concatenation,
    Alternation,
    KleeneStar,
    Skip
  };

  static SymbolType Check(const char c) {
    if (c == '1') return SymbolType::EmptyString;
    if (c == 'a' || c == 'b' || c == 'c') return SymbolType::Literal;
    if (c == '.') return SymbolType::Concatenation;
    if (c == '+') return SymbolType::Alternation;
    if (c == '*') return SymbolType::KleeneStar;
    if (c == ' ') return SymbolType::Skip;

    throw RegExpParserException("Invalid character.");
  }

  template <class TArgument>
  static std::pair<TArgument, TArgument> GetTwoArguments(std::stack<TArgument>& arguments) {
    if (arguments.size() < 2) {
      throw RegExpParserException("Not enough literals.");
    }

    TArgument second_argument = arguments.top();
    arguments.pop();
    TArgument first_argument = arguments.top();
    arguments.pop();

    return {first_argument, second_argument};
  }

  template <class TArgument>
  static TArgument GetArgument(std::stack<TArgument>& arguments) {
    if (arguments.empty()) {
      throw RegExpParserException("Not enough literals.");
    }

    TArgument argument = arguments.top();
    arguments.pop();

    return argument;
  }
};

} // NRegExp
