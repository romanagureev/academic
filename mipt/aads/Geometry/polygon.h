#include "point.h"
#include <vector>
#include <algorithm>

namespace NPlainGeometry {

template <typename Coordinate>
class TPolygon {
 public:
  explicit TPolygon(const std::vector<TPoint<Coordinate>>& points_init)
          : points(points_init) {};

  TPolygon(const TPolygon<Coordinate>& _) = default;

  TPolygon<Coordinate> operator-() const {
    TPolygon<Coordinate> new_polygon(*this);

    for (size_t i = 0; i < new_polygon.GetSize(); ++i) {
      new_polygon.points[i] = -new_polygon.points[i];
    }

    return new_polygon;
  }

  size_t GetSize() const { return points.size(); }

  // I call corner leftmost lowest vertex
  size_t GetCornerIndex() const {
    assert(GetSize() > 0);

    size_t corner_point_i = 0;
    for (size_t i = 1; i < GetSize(); ++i) {
      if (points[i].x < points[corner_point_i].x ||
          (points[i].x == points[corner_point_i].x &&
           points[i].y < points[corner_point_i].y)) {
        corner_point_i = i;
      }
    }

    return corner_point_i;
  }

  TPoint<Coordinate> GetCornerPoint() const {
    return points[GetCornerIndex()];
  }

  void RotateToCorner() {
    size_t shift = GetCornerIndex();

    std::rotate(points.begin(), points.begin() + shift, points.end());
  }

  void Move(const TPoint<Coordinate>& point) {
    for (size_t i = 0; i < GetSize(); ++i) {
      points[i] -= point;
    }
  }

  TSegment<Coordinate> GetSegment(size_t i) const {
    size_t first_point = (i % GetSize());
    size_t second_point = ((first_point + 1) % GetSize());

    return TSegment<Coordinate>(points[first_point], points[second_point]);
  }

  TSegment<Coordinate> GetSortedSegment(size_t i) const {
    auto segment = GetSegment(i);

    if (!CompareByCoordinates(segment.first, segment.second)) {
      std::swap(segment.first, segment.second);
    }

    return segment;
  }
  
  bool ContainForConvex(TPoint<Coordinate> point);

  std::vector<TPoint<Coordinate>> points;
};

}  // NPlainGeometry
