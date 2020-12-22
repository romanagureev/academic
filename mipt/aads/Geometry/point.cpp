#include <point.h>

namespace NPlainGeometry {

/*----------------------------------- In/out ---------------------------------*/

template <typename Coordinate>
TPoint<Coordinate> ReadPointFromStream(std::istream& in_stream) {
  Coordinate x;
  Coordinate y;
  in_stream >> x >> y;
  return TPoint<Coordinate>(x, y);
}

template <typename Coordinate>
std::ostream& operator<<(std::ostream& out_stream,
                         const TPoint<Coordinate>& point) {
    return out_stream << "Point(x=" << point.x << ", y=" << point.y << ")\n";
}

/*--------------------------------- Operators --------------------------------*/

template <typename Coordinate>
bool operator==(const TPoint<Coordinate>& first,
                const TPoint<Coordinate>& second) {
  return (first.x == second.x) &&
         (first.y == second.y);
}

template <typename Coordinate>
bool operator!=(const TPoint<Coordinate>& first,
                const TPoint<Coordinate>& second) {
  return !(first == second);
}

template <typename Coordinate>
const TPoint<Coordinate> operator*(const TPoint<Coordinate>& point,
                                   const Coordinate number) {
  return TPoint<Coordinate>(point.x * number, point.y * number);
}

template <typename Coordinate>
const TPoint<Coordinate> operator/(const TPoint<Coordinate>& point,
                                   const Coordinate number) {
  return TPoint<Coordinate>(point.x / number, point.y / number);
}

template <typename Coordinate>
const TPoint<Coordinate> operator+(const TPoint<Coordinate>& first,
                                   const TPoint<Coordinate>& second) {
  return TPoint<Coordinate>(first.x + second.x, first.y + second.y);
}

template <typename Coordinate>
const TPoint<Coordinate> operator-(const TPoint<Coordinate>& first,
                                   const TPoint<Coordinate>& second) {
  return TPoint<Coordinate>(first.x - second.x, first.y - second.y);
}

/*-------------------------------- Comparators -------------------------------*/

template <typename Coordinate>
bool CompareByCoordinates(const TPoint<Coordinate>& first,
                          const TPoint<Coordinate>& second) {
  return (first.x < second.x) ||
         (first.x == second.x && first.y < second.y);
}

template <typename Coordinate>
struct TCompareByCoordinates {
  bool operator()(const TPoint<Coordinate>& first,
                  const TPoint<Coordinate>& second) const {
    return CompareByCoordinates(first, second);
  }
};

/*--------------------------------- Functions --------------------------------*/

template <typename Coordinate>
Coordinate CrossProduct(const TPoint<Coordinate>& first,
                        const TPoint<Coordinate> second) {
  return first.x * second.y - first.y * second.x;
}

template <typename Coordinate>
Coordinate DotProduct(const TPoint<Coordinate>& first,
                      const TPoint<Coordinate> second) {
  return first.x * second.x + first.y * second.y;
}

enum class ORIENTATION {
  NEGATIVE = -1,
  POSITIVE = 1,
  ZERO = 0
};

template <typename Coordinate>
ORIENTATION Orientation(const TPoint<Coordinate>& first,
                        const TPoint<Coordinate>& second) {
  Coordinate area = CrossProduct(first, second);
  if (area > 0) {
    return ORIENTATION::POSITIVE;
  }
  if (area < 0) {
    return ORIENTATION::NEGATIVE;
  }
  return ORIENTATION::ZERO;
}

}  // NPlainGeometry
