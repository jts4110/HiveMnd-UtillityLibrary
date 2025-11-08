#pragma once
#include <cmath>
#include <iostream>

// -----------------------------------------------------------------------------
//  Class   : HiveMnd::Math::Vector2
//  Purpose : Represents a 2D vector with x and y components and provides
//            arithmetic, normalization, distance, and utility operations.
//  Author  : HiveMnd CodeBase
//  License : (c) HiveMnd LLC - Use of this code without written permission
//             is strictly prohibited.
// -----------------------------------------------------------------------------

namespace HiveMnd::Math
{
    class Vector2
    {
    public:
        float x;
        float y;

        // Constructors ---------------------------------------------------------
        Vector2() : x(0.0f), y(0.0f) {}
        Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

        // Operator overloads ---------------------------------------------------
        Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
        Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
        Vector2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
        Vector2 operator/(float scalar) const { return { x / scalar, y / scalar }; }

        Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }

        bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Vector2& other) const { return !(*this == other); }

        // Magnitude / Length ---------------------------------------------------
        float Length() const { return std::sqrt(x * x + y * y); }

        // Normalize the vector to unit length
        Vector2 Normalized() const
        {
            float len = Length();
            return (len > 0.0f) ? Vector2(x / len, y / len) : Vector2(0.0f, 0.0f);
        }

        // Dot Product ----------------------------------------------------------
        float Dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        // Distance between two vectors -----------------------------------------
        float Distance(const Vector2& other) const
        {
            float dx = x - other.x;
            float dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        // Clamp vector within min/max bounds -----------------------------------
        static Vector2 Clamp(const Vector2& value, const Vector2& min, const Vector2& max)
        {
            return Vector2(
                std::fmax(min.x, std::fmin(value.x, max.x)),
                std::fmax(min.y, std::fmin(value.y, max.y))
            );
        }

        // Linear interpolation between two vectors -----------------------------
        static Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
        {
            return a + (b - a) * t;
        }

        // Print helper ---------------------------------------------------------
        void Print() const
        {
            std::cout << "Vector2(" << x << ", " << y << ")";
        }
    };
}

