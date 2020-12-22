#include <limits>
#include <algorithm>
#include "regexp.h"


namespace NRegExp {

struct TSuffixRegExp {
  static const size_t kInfLength = std::numeric_limits<size_t>::max();
  static TChar counting_char;
  size_t max_one_char_re_len;
  size_t max_suffix_len;
  bool has_other_characters;

  TSuffixRegExp()
      : max_one_char_re_len(0),
       max_suffix_len(0),
       has_other_characters(false) {};

  explicit TSuffixRegExp(const TChar c) {
    if (c == counting_char) {
      max_one_char_re_len = 1;
      max_suffix_len = 1;
      has_other_characters = false;
    } else {
      max_one_char_re_len = 0;
      max_suffix_len = 0;
      has_other_characters = true;
    }
  }

  TSuffixRegExp(size_t length, size_t max_len, bool has_l_c)
      : max_one_char_re_len(length),
        max_suffix_len(max_len),
        has_other_characters(has_l_c) {};

  TSuffixRegExp(const TSuffixRegExp& suffix_reg_exp) = default;

  TSuffixRegExp KleeneStar();
};

} // NRegExp
