#include "segments.h"

namespace NPlainGeometry {

/*----------------------------------- In/out ---------------------------------*/

template <typename Coordinate>
TSegment<Coordinate> ReadSegmentFromStream(std::istream& in_stream) {
    TPoint<Coordinate> first_point = ReadPointFromStream<Coordinate>(in_stream);
    TPoint<Coordinate> second_point = ReadPointFromStream<Coordinate>(in_stream);

    return TSegment<Coordinate>(first_point, second_point);
}

/*--------------------------------- Functions --------------------------------*/

template <typename Coordinate>
bool IsOnSegment(const TPoint<Coordinate>& point,
                 const TSegment<Coordinate>& segment) {
  if (segment.IsPoint()) {
    return point == segment.first;
  }

  return CrossProduct(point - segment.first, point - segment.second) == 0 &&
         DotProduct(point - segment.first, point - segment.second) <= 0;
}

template <typename Coordinate>
bool Intersect(TSegment<Coordinate>& first_segment,
               TSegment<Coordinate>& second_segment) {
  if (first_segment.IsPoint()) {
    return IsOnSegment(first_segment.first, second_segment);
  }
  if (second_segment.IsPoint()) {
    return IsOnSegment(second_segment.first, first_segment);
  }

  // End of segment belongs to another segment
  if (IsOnSegment(first_segment.first, second_segment) ||
      IsOnSegment(first_segment.second, second_segment) ||
      IsOnSegment(second_segment.first, first_segment) ||
      IsOnSegment(second_segment.second, first_segment)) {
    return true;
  }

  return Orientation(first_segment.second - first_segment.first,
                     second_segment.first - first_segment.first) !=
         Orientation(first_segment.second - first_segment.first,
                     second_segment.second - first_segment.first) &&
         Orientation(second_segment.second - second_segment.first,
                     first_segment.first - second_segment.first) !=
         Orientation(second_segment.second - second_segment.first,
                     first_segment.second - second_segment.first);

}

}  // NPlainGeometry
