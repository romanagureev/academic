#include "suffix_regexp.h"


namespace NRegExp {

TChar TSuffixRegExp::counting_char;

template <>
TSuffixRegExp operator|<TSuffixRegExp>(
    const TSuffixRegExp& first_re,
    const TSuffixRegExp& second_re
  ) {
  TSuffixRegExp result;
  result.max_suffix_len = std::max(first_re.max_suffix_len,
                                   second_re.max_suffix_len);
  result.has_other_characters = first_re.has_other_characters &&
                                 second_re.has_other_characters;

  if (!first_re.has_other_characters) {
    result.max_one_char_re_len = first_re.max_one_char_re_len;
  }
  if (!second_re.has_other_characters) {
    result.max_one_char_re_len = std::max(result.max_one_char_re_len, second_re.max_one_char_re_len);
  }

  return result;
}

template <>
TSuffixRegExp operator&<TSuffixRegExp>(
    const TSuffixRegExp& first_re,
    const TSuffixRegExp& second_re
  ) {
  auto kInfLength = TSuffixRegExp::kInfLength;

  TSuffixRegExp result;
  result.max_suffix_len = second_re.max_suffix_len;
  result.has_other_characters = first_re.has_other_characters ||
                                 second_re.has_other_characters;
  if (!second_re.has_other_characters) {
    if (second_re.max_one_char_re_len == kInfLength) {
      result.max_suffix_len = kInfLength;
    } else {
      if (first_re.max_suffix_len != kInfLength) {
        result.max_suffix_len =
          std::max(result.max_suffix_len,
                   first_re.max_suffix_len + second_re.max_one_char_re_len);
      } else {
        result.max_suffix_len = kInfLength;
      }

      if (!first_re.has_other_characters) {
        if (first_re.max_one_char_re_len != kInfLength) {
          result.max_suffix_len =
            std::max(result.max_suffix_len,
                     first_re.max_one_char_re_len + second_re.max_one_char_re_len);
        } else {
          result.max_suffix_len = kInfLength;
        }
      }
    }
  }

  if (!result.has_other_characters) {
    if (first_re.max_one_char_re_len == kInfLength ||
      second_re.max_one_char_re_len == kInfLength) {
      result.max_one_char_re_len = kInfLength;
    } else {
      result.max_one_char_re_len = first_re.max_one_char_re_len + second_re.max_one_char_re_len;
    }
  }

  return result;
}

TSuffixRegExp TSuffixRegExp::KleeneStar() {
  if (!has_other_characters && max_one_char_re_len > 0) {
    return TSuffixRegExp(TSuffixRegExp::kInfLength,
                         max_suffix_len,
                         has_other_characters);
  } else {
    return TSuffixRegExp(this->max_one_char_re_len,
                         this->max_suffix_len,
                         false);
  }
}

} // NRegExp
