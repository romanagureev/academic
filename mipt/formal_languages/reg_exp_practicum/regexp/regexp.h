#include "../regexp.h"


namespace NRegExp {

/* Parser compatibility
class RegExp {
  virtual RegExp();
  virtual RegExp(TChar c);
  virtual RegExp KleeneStar();
};
*/

template <typename TRegExp>
TRegExp operator|(const TRegExp& first_re, const TRegExp& second_re);

template <typename TRegExp>
TRegExp operator&(const TRegExp& first_re, const TRegExp& second_re);

} // NRegExp
