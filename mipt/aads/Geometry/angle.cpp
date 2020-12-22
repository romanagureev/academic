#include "angle.h"

namespace NPlainGeometry {

template <typename Coordinate>
bool CompareByAngle(const TPoint<Coordinate>& first,
                    const TPoint<Coordinate>& second) {
  auto angle_of_first = Angle(first);
  auto angle_of_second = Angle(second);

  if (angle_of_first.x == 0 && angle_of_second.x == 0) {
    return angle_of_first.y < angle_of_second.y;
  }

  return CrossProduct(angle_of_first, angle_of_second) > 0;
}

}  // NPlainGeometry
