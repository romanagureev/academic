#include "polygon.h"
#include "../AVL.cpp"

namespace NPlainGeometry {

/*------------------------------- Initializing -------------------------------*/

template <typename Coordinate>
TPolygon<Coordinate> ReadPolygonFromStream(std::istream& in_stream) {
  size_t points_count = 0;
  in_stream >> points_count;
  std::vector< TPoint<Coordinate> > points;
  points.reserve(points_count);
  for (size_t i = 0; i < points_count; ++i) {
    points.emplace_back(ReadPointFromStream<Coordinate>(in_stream));
    if (i > 0 && points[i - 1] == points[i])
      points.pop_back();
  }

  return TPolygon<Coordinate>(points);
}

template <typename Coordinate>
TPolygon<Coordinate> ReadPolygonClockwiseFromStream(std::istream& in_stream) {
  size_t points_count = 0;
  in_stream >> points_count;

  std::vector<TPoint<Coordinate>> points(points_count);
  for (size_t i = points_count; i > 0; --i) {
    points[i - 1] = ReadPointFromStream<Coordinate>(in_stream);
  }

  return TPolygon<Coordinate>(points);
}

/*---------------------------------- Methods ---------------------------------*/

// Using binary search
// Time complexity: O(logn)
template <typename Coordinate>
bool TPolygon<Coordinate>::ContainForConvex(TPoint<Coordinate> point) {
  TPoint<Coordinate> corner = GetCornerPoint();
  point -= corner;
  Move(corner);
  RotateToCorner();

  bool is_inside = false;
  if (point.x >= 0) {
    if (point.x == 0 && point.y == 0) {
      is_inside = true;
    } else {
      auto iter = std::upper_bound(points.begin() + 1, points.end(), point);
      if (iter == points.end() &&
          point.y == points[GetSize() - 1].y &&
          point.x == points[GetSize() - 1].x) {
        iter = points.end() - 1;
      }
      if (iter != points.begin() + 1 && iter != points.end()) {
        int ray_index = iter - points.begin();
        is_inside = IsInTriangle(points[ray_index - 1],
                                 points[ray_index],
                                 point);
      }
    }
  }

  return is_inside;
}

/*--------------------------------- Functions --------------------------------*/

template <typename Coordinate>
bool IsInTriangle(const TPoint<Coordinate>& a,
                  const TPoint<Coordinate>& b,
                  const TPoint<Coordinate>& c) {
  TSegment<Coordinate> first_segment(a, b);
  TSegment<Coordinate> second_segment(TPoint<Coordinate>(), c);

  return !Intersect(first_segment, second_segment) ||
         IsOnSegment(c, first_segment);
}

// Time and space complexity: O(m + n)
template <typename Coordinate>
TPolygon<Coordinate> GetMinkowskiAddition(TPolygon<Coordinate> first_polygon,
                                          TPolygon<Coordinate> second_polygon) {
  first_polygon.RotateToCorner();
  second_polygon.RotateToCorner();

  uint32_t i = 0;
  uint32_t j = 0;

  std::vector<TPoint<Coordinate>> new_polygon_points;
  while (i < first_polygon.GetSize() || j < second_polygon.GetSize()) {
    new_polygon_points.push_back(first_polygon.points[i] + second_polygon.points[j]);

    TPoint<Coordinate> first_next_edge = first_polygon.points[(i + 1) % first_polygon.GetSize()] -
                                         first_polygon.points[i % first_polygon.GetSize()];
    TPoint<Coordinate> second_next_edge = second_polygon.points[(j + 1) % second_polygon.GetSize()] -
                                          second_polygon.points[j % second_polygon.GetSize()];

    if (first_next_edge < second_next_edge) {
      ++i;
    } else if (first_next_edge > second_next_edge) {
      ++j;
    } else {
      ++i;
      ++j;
    }
  }

  return TPolygon<Coordinate>(new_polygon_points);
}

// Using Graham algorithm
// Time complexity: O(|points| * log(|points|))
// Space complexity: O(|points|)
template <typename Coordinate>
TPointsSet<Coordinate> GetConvexHull(TPointsSet<Coordinate>& points) {
  points.RotateToCorner();
  auto corner_point = points[0];
  points.Move(corner_point);
  std::sort(points.points.begin() + 1,
            points.points.end(),
            CompareByAngle<Coordinate>);

  if (points.GetSize() < 4) {
    return TPointsSet<Coordinate>(points);
  }

  size_t last = 1;
  for (int i = 2; i < points.GetSize(); ++i) {
    while (CompareByAngle(points[i] - points[last],
                          points[last] - points[last - 1])) {
      --last;
    }
    std::swap(points[last + 1], points[i]);
    ++last;
  }

  points.Move(-corner_point);

  std::vector<TPoint<Coordinate>> convex_hull_points;
  convex_hull_points.reserve(last + 1);
  std::copy(points.points.begin(),
            points.points.begin() + last + 1,
            std::back_inserter(convex_hull_points));

  return TPointsSet<Coordinate>(convex_hull_points);
}

enum class ContainType {
  INSIDE,
  BORDER,
  OUTSIDE,
  UNDEFINED
};

// Auxilary functions for containment check
namespace NContain {
  enum EventType {
    START,
    END,
    SEARCHING_POINT
  };

  template <typename Coordinate>
  struct Event {
    TPoint<Coordinate> point;
    EventType type;
    size_t index;

    explicit Event(TPoint<Coordinate> point_init,
                   EventType type_init,
                   size_t index_init)
            : point(point_init), type(type_init), index(index_init) {};
  };

  template <typename Coordinate>
  bool EventCompare(const Event<Coordinate>& first,
                    const Event<Coordinate>& second) {
    if (first.point.x != second.point.x) {
      return first.point.x < second.point.x;
    }
    if (first.type != second.type) {
      return first.type < second.type;
    }
    return first.point.y < second.point.y;
  }

  template<typename Coordinate>
  void SetEventLine(std::vector<Event<Coordinate>>& events,
                    const TPolygon<Coordinate>& polygon,
                    const std::vector<TPoint<Coordinate>>& points) {
    events.reserve(points.size() + polygon.GetSize() * 2);
    for (size_t i = 0; i < points.size(); ++i) {
      events.emplace_back(points[i], EventType::SEARCHING_POINT, i);
    }
    for (size_t i = 0; i < polygon.GetSize(); ++i) {
      auto segment = polygon.GetSortedSegment(i);
      events.emplace_back(segment.first, EventType::START, i);
      events.emplace_back(segment.second, EventType::END, i);
    }

    std::sort(events.begin(), events.end(), EventCompare<Coordinate>);
  }

  // Works for not vertical
  template <typename Coordinate>
  struct ObjectCompare {
    bool operator()(TSegment<Coordinate> first, TSegment<Coordinate> second) {
      if (first.IsPoint()) {
        return Orientation(second.second - second.first, first.first - second.first) == ORIENTATION::NEGATIVE;
      } else if (second.IsPoint()) {
        return Orientation(first.second - first.first, second.first - first.first) == ORIENTATION::POSITIVE;
      } else {
        if (first.first == second.first) {
          return Orientation(first.second - first.first, second.second - first.first) == ORIENTATION::POSITIVE;
        }
        if (first.first == second.second) {
          return false;
        }
        if (first.second == second.first) {
          return true;
        }
        if (first.second == second.second ||
            first.first.x <= second.first.x && second.first.x <= first.second.x) {
          return Orientation(first.second - first.first, second.first - first.first) == ORIENTATION::POSITIVE;
        }
        return Orientation(second.second - second.first, first.first - second.first) == ORIENTATION::NEGATIVE;
      }
    }
  };

  template <typename Coordinate>
  TSegment<Coordinate> GetSegmentOfPoint(const TPoint<Coordinate>& point) {
    return TSegment<Coordinate>(point, point);
  }

  template <typename Coordinate>
  void ProcessEvents(const std::vector<Event<Coordinate>>& events,
                     const TPolygon<Coordinate>& polygon,
                     std::vector<ContainType>& verdicts) {
    // Inits
    AVL<TSegment<Coordinate>, ObjectCompare<Coordinate>> current_segments;

    for (auto event: events) {
      if (event.type == EventType::START) {
        auto segment = polygon.GetSortedSegment(event.index);
        if (segment.first.x != segment.second.x) {
          current_segments.Insert(segment);
        }
      } else if (event.type == EventType::END) {
        auto seg = polygon.GetSortedSegment(event.index);
        if (seg.first.x != seg.second.x) {
          current_segments.Remove(seg);
        }
      } else if (event.type == EventType::SEARCHING_POINT) {
        size_t order = current_segments.FindIndexOfElement(GetSegmentOfPoint(event.point));
        verdicts[event.index] = (order % 2 == 0) ? ContainType::OUTSIDE : ContainType::INSIDE;

        // Check if lies on segment
        if (order != 0 && IsOnSegment(event.point, current_segments.FindKthElement(order))) {
          verdicts[event.index] = ContainType::BORDER;
        }
      }
    }
  }

  template <typename Coordinate>
  void ProcessVerticalEvents(const std::vector<Event<Coordinate>>& events,
                     const TPolygon<Coordinate>& polygon,
                     std::vector<ContainType>& verdicts) {
    // Inits
    AVL<TPoint<Coordinate>, ComparePointsByCoordinates<Coordinate>> vertical_segments;

    Coordinate previous_x;
    if (!events.empty()) {
      previous_x = events[0].point.x;
    }

    for (auto event: events) {
      if (event.point.x != previous_x) {
        vertical_segments.Clear();
        previous_x = event.point.x;
      }

      if (event.type == EventType::START) {
        auto segment = polygon.GetSortedSegment(event.index);
        if (segment.first.x == segment.second.x) {
          vertical_segments.Insert(segment.first);
          vertical_segments.Insert(segment.second);
        }
      } else if (event.type == EventType::SEARCHING_POINT) {
        size_t order = vertical_segments.FindIndexOfElement(event.point);
        if (order % 2 == 1 || (order != 0 && event.point == vertical_segments.FindKthElement(order))) {
          verdicts[event.index] = ContainType::BORDER;
        }
      }
    }
  }

  template <typename Coordinate>
  void CheckPolygonVerticesIntersection(const TPolygon<Coordinate>& polygon,
                                        const std::vector<TPoint<Coordinate>>& points,
                                        std::vector<ContainType>& verdicts) {
    std::set<TPoint<Coordinate>, ComparePointsByCoordinates<Coordinate>> vertices;

    for (auto point: polygon.points) {
      vertices.insert(point);
    }

    for (size_t i = 0; i < points.size(); ++i) {
      if (vertices.count(points[i]) > 0) {
        verdicts[i] = ContainType::BORDER;
      }
    }
  }
} // NContain

template <typename Coordinate>
std::vector<ContainType> Contain(const TPolygon<Coordinate>& polygon,
                                 const std::vector<TPoint<Coordinate>>& points) {
  std::vector<Event<Coordinate>> events
  NContain::SetEventLine<Coordinate>(events, polygon, points);

  std::vector<ContainType> verdicts(points.size(), ContainType::UNDEFINED);
  NContain::ProcessEvents(events, polygon, verdicts);
  NContain::ProcessVerticalEvents(events, polygon, verdicts);

  NContain::CheckPolygonVerticesIntersection(polygon, points, verdicts);

  return verdicts;
}

}  // NPlainGeometry
