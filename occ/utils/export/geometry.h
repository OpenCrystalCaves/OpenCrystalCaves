#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "vector.h"

namespace geometry
{

// Represents a position in 2d space
using Position = Vector<int>;

// Represents an objects size (x is width, y is height)
using Size = Vector<int>;

struct Rectangle
{
  constexpr Rectangle() : position(), size() {}

  constexpr Rectangle(Position position, Size size) : position(std::move(position)), size(std::move(size)) {}

  constexpr Rectangle(Position position, int width, int height) : position(std::move(position)), size(width, height) {}

  constexpr Rectangle(int x, int y, Size size) : position(x, y), size(std::move(size)) {}

  constexpr Rectangle(int x, int y, int width, int height) : position(x, y), size(width, height) {}

  Position position;
  Size size;

  constexpr Rectangle operator+(const Position& p) const { return Rectangle(position + p, size); }

  constexpr Rectangle operator-(const Position& p) const { return Rectangle(position - p, size); }
};

// Returns true if Rectangle a and Rectangle b intersect
constexpr bool isColliding(const Rectangle& a, const Rectangle& b)
{
  return a.position.x() < b.position.x() + b.size.x() && a.position.y() < b.position.y() + b.size.y() &&
    a.position.x() + a.size.x() > b.position.x() && a.position.y() + a.size.y() > b.position.y();
}
constexpr bool is_any_colliding(const std::vector<Rectangle>& v, const Rectangle& a)
{
  struct Collides
  {
    const Rectangle& r;
    constexpr Collides(const Rectangle& r) : r(r) {}
    constexpr bool operator()(const Rectangle& r2) const { return isColliding(r, r2); }
  };
  return std::any_of(v.cbegin(), v.cend(), Collides(a));
}
// Returns true if A is within B
constexpr bool is_inside(const Rectangle& a, const Rectangle& b)
{
  return a.position.x() >= b.position.x() && a.position.y() >= b.position.y() &&
    a.position.x() + a.size.x() <= b.position.x() + b.size.x() && a.position.y() + a.size.y() <= b.position.y() + b.size.y();
}

}
