#include "pointer.h"

namespace NPlainGeometry {

template <typename Coordinate>
class TSegment {
 public:
  explicit TSegment(TPoint<Coordinate> first_init,
                    TPoint<Coordinate> second_init)
          : first(first_init), second(second_init) {};

  explicit TSegment()
          : first(TPoint<Coordinate>()),
            second(TPoint<Coordinate>()) {};

  [[nodiscard]] bool IsPoint() const { return first == second; }

  TPoint<Coordinate> first;
  TPoint<Coordinate> second;
};

}  // NPlainGeometry
