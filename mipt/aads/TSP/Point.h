#ifndef TSP_POINT_H
#define TSP_POINT_H

#include <math.h>

template <class T = long double>
struct Point {
  T x;
  T y;

  Point() : x(0), y(0) {}
  Point(T x_init, T y_init) : x(x_init), y(y_init) {}

  friend Point<T> operator-(const Point<T>& first, const Point<T>& second) {
    return Point<T>(first.x - second.x, first.y - second.y);
  }

  long double GetLength() const { return sqrt(x * x + y * y); }

  friend long double GetLength(const Point<T>& first, const Point<T>& second) {
    return (first - second).GetLength();
  }
};

#endif  // TSP_POINT_H
