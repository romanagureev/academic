#include "parser.cpp"
#include "regexp/suffix_regexp.h"


namespace NRegExp {

bool ContainsSuffix(std::string& reg_exp, TChar letter, size_t suffix_length) {
  TSuffixRegExp::counting_char = letter;
  TSuffixRegExp re = TParser::Parse<TSuffixRegExp>(reg_exp);

  return re.max_suffix_len >= suffix_length ||
         (!re.has_other_characters && re.max_one_char_re_len >= suffix_length);
}

}  // NRegExp
