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
#include <cmath>

#include "utils.h"
#include "coremaths.h"
#include "display.h"

using namespace std;

///
/// Vector2
///

Vector2::Vector2(float _x, float _y)
{
    x = _x;
    y = _y;
}

float Vector2::LengthSquared()
{
    return (x * x) + (y * y);
}

float Vector2::Length()
{
    return sqrt(LengthSquared());
}

float Vector2::Dot(Vector2 vec)
{
    return (x * vec.x) + (y * vec.y);
}

Vector2 Vector2::Normalized()
{
    if (x == 0.0f && y == 0.0f)
    {
        return *this;
    }
    return (*this) * (1.0f / Length());
}

Vector2 Vector2::ProjectOnto(Vector2 vec)
{
    return vec * (Dot(vec) / vec.LengthSquared());
}

float Vector2::DistanceSquared(Vector2 point)
{
    point -= (*this);
    return point.LengthSquared();
}

float Vector2::Distance(Vector2 point)
{
    point -= (*this);
    return point.Length();
}

Vector2 Vector2::Reflection(Vector2 normal)
{
    return (*this) - (2.0f * ProjectOnto(normal));
}

Vector2 Vector2::Rotation90Clockwise()
{
    Vector2 output;
    output.x = y;
    output.y = -x;
    return output;
}

void Vector2::Rotate90Clockwise()
{
    float cachex = x;
    x = y;
    y = -cachex;
}

Vector2 Vector2::Rotation90AntiClockwise()
{
    return Vector2(-y, x);
}

void Vector2::Rotate90AntiClockwise()
{
    float cachex = x;
    x = -y;
    y = cachex;
}

Vector2 Vector2::Lerp(Vector2 vec, float w)
{
    w = Clamp(w, 0.0f, 1.0f);
    return ((*this) * (1.0f - w)) + (vec * w);
}

float Vector2::RotationRad()
{
    return atan2(x, y);
}

float Vector2::Rotation()
{
    return RotationRad() * (180.0f / pi);
}

void Vector2::Rotate(float degrees)
{
    RotateRad((pi / 180.0f) * degrees);
}

void Vector2::RotateRad(float radians)
{
    float length = Length();
    x = sin(radians) * length;
    y = cos(radians) * length;
}

Vector2 Vector2::Rotation(float degrees)
{
    Vector2 v = *this;
    v.Rotate(degrees);
    return v;
}

Vector2 Vector2::RotationRad(float radians)
{
    Vector2 v = *this;
    v.RotateRad(radians);
    return v;
}

Vector2 Vector2::Max(Vector2 vec)
{
    return Vector2(max(x, vec.x), max(y, vec.y));
}

Vector2 Vector2::Min(Vector2 vec)
{
    return Vector2(min(x, vec.x), min(y, vec.y));
}

const Vector2 Vector2::Zero         =   {0, 0};
const Vector2 Vector2::OneOne       =   {1, 1};
const Vector2 Vector2::OneNegOne    =   {1, -1};
const Vector2 Vector2::OneZero      =   {1, 0};
const Vector2 Vector2::ZeroOne      =   {0, 1};
const Vector2 Vector2::NegOneNegOne =   {-1, -1};
const Vector2 Vector2::NegOneZero   =   {-1, 0};
const Vector2 Vector2::ZeroNegOne   =   {0, -1};

///
/// Point
///

Point::Point(float _x, float _y)
{
    x = _x;
    y = _y;
}

Point::Point(const Vector2& vec)
{
    x = vec.x;
    y = vec.y;
}

void Point::Draw(Display& display)
{
    Draw(display, display.GetDrawColor());
}

void Point::Draw(Display& display, uint16_t color)
{
    display.GetTFT()->drawPixel((int)x, (int)y, color);
}

bool Point::Intersects(Circle circle)
{
    return DistanceSquared((Vector2){circle.x, circle.y}) <= circle.r * circle.r;
}
bool Point::Intersects(InfiniteLine infiniteLine)
{
    /// Treat this as a point on the line and check if it has the same Y-intercept
    float m = (infiniteLine.u.y / infiniteLine.u.x);
    return (y - (m * x)) == (infiniteLine.p.y - (m * infiniteLine.p.x));
}
bool Point::Intersects(Line line)
{
    /// Similar to above, but within a limited range
    InfiniteLine infLine = {line.a, (Vector2)line.b};
    return Intersects(infLine) && line.a.DistanceSquared(*this) <= line.a.DistanceSquared(line.b);
}
bool Point::Intersects(Ray ray)
{
    /// Same as infinite line but we also check if the point is on the correct side
    InfiniteLine line = {ray.p, static_cast<Point>(ray.u)};
    return Intersects(line) && ray.u.Dot(*this) > 0.0;
}
bool Point::Intersects(Rect rect)
{
    return x >= rect.x && x < rect.xmax() && y >= rect.y && y < rect.ymax();
}

///
/// Ray
///

Ray::Ray(Point origin, Vector2 direction)
{
    p = origin;
    u = direction;
}

///
/// InfiniteLine
///

InfiniteLine::InfiniteLine(Point p, Vector2 direction)
{
    p = p;
    u = direction;
}

///
/// Circle
///

/// TODO: use a more efficient drawing algorithm
void Circle::Draw(Display& display)
{
    Draw(display, display.GetDrawColor());
}

void Circle::Draw(Display& display, uint16_t color)
{
    display.GetTFT()->drawCircle((int)x, (int)y, (int)r, color);
}

void Circle::DrawFilled(Display& display)
{
    DrawFilled(display, display.GetDrawColor());
}

void Circle::DrawFilled(Display& display, uint16_t color)
{
    display.GetTFT()->fillCircle((int)x, (int)y, (int)r, color);
}

bool Circle::Intersects(Circle circle)
{
    float totalRadius = (r * circle.r);
    return Point(x, y).DistanceSquared(Point(circle.x, circle.y)) < totalRadius * totalRadius;
}
bool Circle::Intersects(InfiniteLine infiniteLine)
{
    /// TODO
    return false;
}
bool Circle::Intersects(Line line)
{
    /// TODO
    return false;
}
bool Circle::Intersects(Ray ray)
{
    /// TODO
    return false;
}
bool Circle::Intersects(Rect rect)
{
    Point offset;
    if (x < rect.x)
    {
        offset.x = rect.x;
    }
    else if (x > rect.xmax())
    {
        offset.x = rect.xmax();
    }
    else
    {
        offset.x = x;
    }
    if (y < rect.y)
    {
        offset.y = rect.y;
    }
    else if (y > rect.ymax())
    {
        offset.y = rect.ymax();
    }
    else
    {
        offset.y = y;
    }
    return (Point(x, y).DistanceSquared(offset) < r * r);
}
bool Circle::Contains(Point point)
{
    return point.Intersects(*this);
}

///
/// Line
///

Line::Line(Point start, Point end)
{
    a = start;
    b = end;
}

void Line::Draw(Display& display)
{
    Draw(display, display.GetDrawColor());
}

void Line::Draw(Display& display, uint16_t color)
{
    display.GetTFT()->drawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
}

///
/// Rect
///

Rect::Rect()
{
}

Rect::Rect(float xpos, float ypos, float width, float height)
{
    x = xpos;
    y = ypos;
    w = width;
    h = height;
}

Rect::Rect(IntRect rect)
{
    x = (float)rect.x;
    y = (float)rect.y;
    w = (float)rect.w;
    h = (float)rect.h;
}

void Rect::DrawFilled(Display& display)
{
    DrawFilled(display, display.GetDrawColor());
}

void Rect::DrawFilled(Display& display, uint16_t color)
{
    IntRect rect = Int();
    display.GetTFT()->fillRect(rect.x, rect.y, rect.w, rect.h, color);
}

void Rect::Draw(Display& display)
{
    Draw(display, display.GetDrawColor());
}

void Rect::Draw(Display& display, uint16_t color)
{
    IntRect rect = Int();
    display.GetTFT()->drawFastHLine(rect.x, rect.y, rect.w, color);
    display.GetTFT()->drawFastVLine(rect.x, rect.y, rect.h, color);
    display.GetTFT()->drawFastVLine(rect.x + rect.w, rect.y, rect.h, color);
    display.GetTFT()->drawFastHLine(rect.x, rect.y + rect.h, rect.w, color);
}

bool Rect::Intersects(Circle circle)
{
    return Point(x, y).Intersects(circle) || Point(x, ymax()).Intersects(circle) || Point(xmax(), y).Intersects(circle) || Point(xmax(), ymax()).Intersects(circle);
}
bool Rect::Intersects(InfiniteLine infiniteLine)
{
    /// TODO
    return false;
}
bool Rect::Intersects(Line line)
{
    /// TODO
    return false;
}
bool Rect::Intersects(Ray ray)
{
    /// TODO
    return false;
}
bool Rect::Intersects(Rect rect)
{
    return !((x > rect.xmax() || xmax() < rect.x) && (y > rect.ymax() || ymax() < rect.y));
}
bool Rect::Contains(Point point)
{
    return point.Intersects(*this);
}

IntRect Rect::Int()
{
    return (IntRect){(int)round(x), (int)round(y), (int)round(w), (int)round(h)};
}

const Rect Rect::Zero = {0, 0, 0, 0};

bool Rect::operator==(const Rect& rect)
{
    return rect.x == x && rect.y == y && rect.w == w && rect.h == h;
}
bool Rect::operator!=(const Rect& rect)
{
    return !(*this == rect);
}

///
/// Polygon
///

void Polygon::DrawFilled(Display& display)
{
    /// TODO: this
}

void Polygon::DrawFilled(Display& display, uint16_t color)
{
    display.SetDrawColor(color);
    DrawFilled(display);
}

void Polygon::Draw(Display& display, uint16_t color)
{
    if (!vertices.empty())
    {
        Point previousPoint = vertices[0];
        for (unsigned int i = 1, counti = vertices.size(); i < counti; i++)
        {
            Line(Point(previousPoint.x, previousPoint.y), Point(vertices[i].x, vertices[i].y)).Draw(display, color);
            previousPoint = vertices[i];
        }
        Line(Point(previousPoint.x, previousPoint.y), Point(vertices[0].x, vertices[0].y)).Draw(display, color);
    }
}

void Polygon::Draw(Display& display)
{
    Draw(display, display.GetDrawColor());
}

Vector2 operator+(const Vector2& vec_a, const Vector2& vec_b)
{
    return {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
}

void operator+=(Vector2 &vec_a, const Vector2& vec_b)
{
    vec_a = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
}

Vector2 operator-(const Vector2& vec_a, const Vector2& vec_b)
{
    return {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
}

void operator-=(Vector2& vec_a, const Vector2& vec_b)
{
    vec_a = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
}

Vector2 operator*(const Vector2& vec_a, const Vector2& vec_b)
{
    return {vec_a.x * vec_b.x, vec_a.y * vec_b.y};
}
