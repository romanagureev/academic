#include <stream.h>

namespace NPlainGeometry {

template <typename Coordinate>
class TPoint {
 public:
  explicit TPoint<Coordinate>(Coordinate x_init, Coordinate y_init)
          : x(x_init), y(y_init) {};

  explicit TPoint<Coordinate>() : x(0), y(0) {};

  TPoint<Coordinate>(const TPoint<Coordinate>& _) = default;

  void operator+=(const TPoint<Coordinate>& point) {
    x += point.x;
    y += point.y;
  }

  void operator-=(const TPoint<Coordinate>& point) {
    x -= point.x;
    y -= point.y;
  }

  void operator*=(const Coordinate number) {
    x *= number;
    y *= number;
  }

  void operator/=(const Coordinate number) {
    x /= number;
    y /= number;
  }

  TPoint<Coordinate> operator-() const {
    TPoint<Coordinate> new_point(*this);

    new_point.x = -x;
    new_point.y = -y;

    return new_point;
  }

  [[nodiscard]] Coordinate GetSquareLength() const {
    return x * x + y * y;
  }

  Coordinate x;
  Coordinate y;
};

}  // NPlainGeometry
