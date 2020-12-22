#include <istream>
#include <map>
#include <set>
#include <string>
#include <vector>

// Context-Free Grammar
namespace NCFGrammar {

template <class CharT = char>
struct TRule {
  CharT start;
  std::basic_string<CharT> end;

  TRule(CharT start_init, const std::basic_string<CharT>& end_init)
    : start(start_init), end(end_init) {}
};

template <class CharT = char>
struct TSituation {
  std::basic_string<CharT> remaining;
  CharT start;
  int pos;

  TSituation(const std::basic_string<CharT>& remaining_init, CharT start_init, int pos_init)
    : remaining(remaining_init), start(start_init), pos(pos_init) {}

  explicit TSituation(const TSituation<CharT>& other, bool used)
    : remaining(""), start(other.start) {
    for (int i = 1; i < other.remaining.size(); ++i) {
      remaining += other.remaining[i];
    }

    pos = other.pos;
  }

  TSituation(const TSituation<CharT>& other) = default;
  TSituation(TSituation<CharT>&& other) = default;

  explicit TSituation(const TRule<CharT>& rule, int i)
    : start(rule.start), remaining(rule.end), pos(i) {}

  bool operator<(const TSituation<CharT>& other) const {
    if (start != other.start) {
      return start < other.start;
    }
    if (pos != other.pos) {
      return pos < other.pos;
    }
    return remaining < other.remaining;
  }
};

// Using Earley algorithm
// Time Complexity: O(n^3)
template <class CharT = char>
class TRecognizer {
 public:
  TRecognizer(CharT start_symbol = 'I') : START_SYMBOL(start_symbol) {}

  void ParseRules(std::basic_istream<CharT>& in_stream) {
    std::basic_string<CharT> start;
    in_stream >> name_ >> start;
    rules.emplace_back(START_SYMBOL, start);
    rules_start = start[0];

    std::basic_string<CharT> from, delimiter, to;
    while ((in_stream >> from) && from != "end") {
      in_stream >> delimiter >> to;

      if (to == EPS) {
        to = "";
      }

      rules.emplace_back(from[0], to);
    }
  }

  const std::basic_string<CharT>& GetName() const {
    return name_;
  }

  bool Check(const std::basic_string<CharT>& pattern) {
    situations.clear();
    situations.resize(pattern.size() + 1);
    situations[0].insert(
        TSituation<CharT>(std::basic_string<CharT>(1, rules_start), START_SYMBOL, 0));

    for (int index = 0; index <= pattern.size(); ++index) {
      Scan(pattern, index);
      size_t prev_size = -1;
      // while changed
      while (prev_size != situations[index].size()) {
        prev_size = situations[index].size();
        Complete(index);
        Predict(index);
      }
    }

    for (auto& situation: situations[pattern.size()]) {
      // START_SYMBOL -> _'
      if (situation.pos == 0 &&
          situation.start == START_SYMBOL &&
          situation.remaining.size() == 0) {
        return true;
      }
    }

    return false;
  }

 private:
  std::vector<TRule<CharT>> rules;
  std::vector<std::set<TSituation<CharT>>> situations;
  std::basic_string<CharT> name_;

  std::basic_string<CharT> EPS = "\\EPS";
  CharT START_SYMBOL;
  CharT rules_start;

  void Scan(const std::basic_string<CharT>& pattern, int index) {
    if (index == 0) {
      return;
    }

    for (auto& situation: situations[index - 1]) {
      if (situation.remaining.size() > 0 &&
          situation.remaining[0] == pattern[index - 1]) {
        situations[index].insert(TSituation<CharT>(situation, true));
      }
    }
  }

  void Predict(int index) {
    std::set<TSituation<CharT>> add;

    for (auto& situation: situations[index]) {
      if (situation.remaining.size() != 0) {
        for (auto& rule: rules) {
          // A -> _'B_, B -> _
          if (rule.start == situation.remaining[0]) {
            add.insert(TSituation<CharT>(rule, index));
          }
        }
      }
    }

    for (auto& i: add) {
      situations[index].insert(i);
    }
  }

  void Complete(int index) {
    std::set<TSituation<CharT>> add;

    for (auto& situation: situations[index]) {
      // A -> _'
      if (situation.remaining.size() == 0) {
        for (auto& other_situation: situations[situation.pos]) {
          // B -> _'A_, A -> _'
          if (!other_situation.remaining.empty() &&
              other_situation.remaining[0] == situation.start) {
            add.insert(TSituation<CharT>(other_situation, true));
          }
        }
      }
    }

    for (auto& i: add) {
      situations[index].insert(i);
    }
  }
};

} // NCFGrammar

