#include "point.h"

namespace NPlainGeometry {

template <typename Coordinate>
class TAngle : TPoint<Coordinate> {
 public:
  explicit TAngle(const TPoint<Coordinate>& point) {
    if (point.y == 0) {
      return TPoint<Coordinate>(point.x < 0 ? -1 : 1, 0);
    }
    return point;
  }
};

}  // NPlainGeometry
