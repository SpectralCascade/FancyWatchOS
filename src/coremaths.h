/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/

/**
 *  PLAINLY MARKED ALTERED SOURCE:
 *  Yoinked from Ossium Engine and modified so it doesn't rely on Box2D.
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <Arduino.h>

class Display;

const float pi = 3.1415927;

inline float RadToDeg(float angle)
{
    return (180.0f / pi) * angle;
}
inline float DegToRad(float angle)
{
    return (pi / 180.0f) * angle;
}

struct IntRect
{
    int x, y, w, h;
};

struct Vector2
{
    float x;
    float y;

    Vector2() = default;
    Vector2(float _x, float _y);

    /// Return the magnitude of this vector.
    float Length();

    /// Return the square magnitude of this vector.
    float LengthSquared();

    /// Dot product of this and another vector
    float Dot(Vector2 vec);

    /// Get a normalized version of this vector.
    Vector2 Normalized();

    /// Vector2 projection of vec_a onto vec_b
    Vector2 ProjectOnto(Vector2 vec);

    /// Calculate distance squared between two point vectors
    float DistanceSquared(Vector2 point);

    /// Calculate distance between two point vectors
    float Distance(Vector2 point);

    /// Calculate reflection of vector
    Vector2 Reflection(Vector2 normal);

    /// Rotate a vector 90 degrees clockwise
    Vector2 Rotation90Clockwise();
    /// Rotate this vector 90 degrees clockwise
    void Rotate90Clockwise();

    /// Rotate a vector 90 degrees clockwise
    Vector2 Rotation90AntiClockwise();
    /// Rotate this vector 90 degrees clockwise
    void Rotate90AntiClockwise();

    /// Calculate some point between two vectors (LERP - linear interpolation)
    Vector2 Lerp(Vector2 vec, float w);

    /// Gets the rotation of this vector in radians
    float RotationRad();
    /// Gets the rotation of this vector in degrees
    float Rotation();
    /// Rotates the vector in degrees
    void Rotate(float degrees);
    /// Rotates the vector in radians
    void RotateRad(float radians);
    /// Returns a vector rotated by degrees
    Vector2 Rotation(float degrees);
    /// Returns a vector rotated by radians
    Vector2 RotationRad(float radians);

    /// Returns a new vector with the biggest of each vector's components.
    Vector2 Max(Vector2 vec);

    /// Returns a new vector with the smallest of each vector's components.
    Vector2 Min(Vector2 vec);

    const static Vector2 Zero;
    const static Vector2 OneOne;
    const static Vector2 OneNegOne;
    const static Vector2 OneZero;
    const static Vector2 ZeroOne;
    const static Vector2 NegOneNegOne;
    const static Vector2 NegOneZero;
    const static Vector2 ZeroNegOne;

};

/// Forward declarations
struct Circle;
struct Ray;
struct Line;
struct InfiniteLine;
struct Triangle;
struct Polygon;
struct Rect;

/// Similar to Vector2, but also implements drawing methods.
struct Point : public Vector2
{
    Point() = default;
    Point(float _x, float _y);
    Point(const Vector2& vec);

    void Draw(Display& display);
    void Draw(Display& display, uint16_t color);

    /// Whether or not this point is intersecting a circle
    bool Intersects(Circle circle);
    /// Whether or not this point is intersecting an infinite line
    bool Intersects(InfiniteLine infiniteLine);
    /// Whether or not this point is intersecting a line
    bool Intersects(Line line);
    /// Whether or not this point is intersecting a ray
    bool Intersects(Ray ray);
    /// Whether or not this point is intersecting a rect
    bool Intersects(Rect rect);

};

struct Circle
{
    float x;
    float y;
    float r;

    void Draw(Display& display);
    void Draw(Display& display, uint16_t color);
    void DrawFilled(Display& display);
    void DrawFilled(Display& display, uint16_t color);

    /// Whether or not this rect is intersecting a circle
    bool Intersects(Circle circle);
    /// Whether or not this rect is intersecting an infinite line
    bool Intersects(InfiniteLine infiniteLine);
    /// Whether or not this rect is intersecting a line
    bool Intersects(Line line);
    /// Whether or not this rect is intersecting a ray
    bool Intersects(Ray ray);
    /// Whether or not this rect is intersecting a rect
    bool Intersects(Rect rect);
    /// Whether or not a point is inside this rect
    bool Contains(Point point);
};

struct Ray
{
    Ray() = default;
    Ray(Point origin, Vector2 direction);

    Point p;
    Vector2 u;
};

struct InfiniteLine : public Ray
{
    InfiniteLine() = default;
    InfiniteLine(Point p, Vector2 direction);
};

struct Line
{
    Line() = default;
    Line(Point start, Point end);

    Point a;
    Point b;

    void Draw(Display& display);
    void Draw(Display& display, uint16_t color);

};

/// Floating point rectangle; if you want an integer based rectangle, use IntRect instead
struct Rect
{
    Rect();
    Rect(float xpos, float ypos, float width, float height);
    Rect(IntRect rect);

    float x;
    float y;
    float w;
    float h;

    void Draw(Display& display);
    void Draw(Display& display, uint16_t color);

    void DrawFilled(Display& display);
    void DrawFilled(Display& display, uint16_t color);

    /// Whether or not this rect is intersecting a circle
    bool Intersects(Circle circle);
    /// Whether or not this rect is intersecting an infinite line
    bool Intersects(InfiniteLine infiniteLine);
    /// Whether or not this rect is intersecting a line
    bool Intersects(Line line);
    /// Whether or not this rect is intersecting a ray
    bool Intersects(Ray ray);
    /// Whether or not this rect is intersecting a rect
    bool Intersects(Rect rect);
    /// Whether or not a point is inside this rect
    bool Contains(Point point);

    inline int xmax()
    {
        return x + w;
    };
    inline int ymax()
    {
        return y + h;
    };

    bool operator==(const Rect& rect);
    bool operator!=(const Rect& rect);

    /// Returns the IntRect equivalent of this rect for convenience
    IntRect Int();

    /// A rect initialised to zero.
    const static Rect Zero;

};

struct Triangle
{
    Point a;
    Point b;
    Point c;

    void Draw(Display& display);
    void Draw(Display& display, uint16_t color);

    void DrawFilled(Display& display);
    void DrawFilled(Display& display, uint16_t color);

};

/// Represents a general shape
struct Polygon
{
    /// Renders the edges of the polygon
    void Draw(Display& display);

    /// Renders the polygon in a solid color
    void DrawFilled(Display& display);

    void Draw(Display& display, uint16_t color);
    void DrawFilled(Display& display, uint16_t color);

    /// The vertices of the polygon
    std::vector<Point> vertices;

};

///
/// Vector2 maths
///

/// Basic vector arithmetic
Vector2 operator+(const Vector2& vec_a, const Vector2& vec_b);

void operator+=(Vector2 &vec_a, const Vector2& vec_b);

Vector2 operator-(const Vector2& vec_a, const Vector2& vec_b);

void operator-=(Vector2 &vec_a, const Vector2& vec_b);

Vector2 operator*(const Vector2& vec_a, const Vector2& vec_b);

/// Scalar multiplication
inline Vector2 operator*(const Vector2& vec, float scalar)
{
    return Vector2(vec.x * scalar, vec.y * scalar);
}
inline Vector2 operator*(float scalar, const Vector2& vec)
{
    return Vector2(vec.x * scalar, vec.y * scalar);
}

#endif // VECTOR_H
