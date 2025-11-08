#pragma once
#include <cmath>
#include <iostream>

// -----------------------------------------------------------------------------
// HiveMnd Utility Library : Core Math Module
// File: Vector3.h
//
// Description:
//  A 3D vector class that provides basic math operations used for movement,
//  physics, rendering, and geometric calculations. This serves as the backbone
//  for position, direction, and normal math in 3D space.
// -----------------------------------------------------------------------------

namespace HiveMnd::Math
{
    class Vector3
    {
    public:
        float x, y, z; // The three spatial components of the vector

        // ---------------------------------------------------------------------
        // Constructors
        // ---------------------------------------------------------------------

        // Default constructor (initializes to zero)
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        // Constructor with specific component values
        Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

        // ---------------------------------------------------------------------
        // Operator Overloads for Basic Math
        // ---------------------------------------------------------------------

        // Adds two vectors together (component-wise)
        Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }

        // Subtracts one vector from another (component-wise)
        Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }

        // Multiplies the vector by a scalar value (scales the vector)
        Vector3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }

        // Divides the vector by a scalar value
        Vector3 operator/(float scalar) const { return { x / scalar, y / scalar, z / scalar }; }

        // Adds another vector’s components to this one
        Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }

        // Subtracts another vector’s components from this one
        Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

        // Checks if two vectors are exactly equal (component-wise)
        bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }

        // Checks if two vectors are not equal
        bool operator!=(const Vector3& other) const { return !(*this == other); }

        // ---------------------------------------------------------------------
        // Magnitude and Normalization
        // ---------------------------------------------------------------------

        // Calculates the vector’s total length (magnitude)
        float Length() const { return std::sqrt(x * x + y * y + z * z); }

        // Returns a new normalized copy (unit vector version)
        Vector3 Normalized() const
        {
            float len = Length();
            // Prevent divide-by-zero if the vector length is 0
            return (len > 0.0f) ? Vector3(x / len, y / len, z / len) : Vector3(0.0f, 0.0f, 0.0f);
        }

        // Converts this vector into a normalized one in place
        void Normalize()
        {
            float len = Length();
            if (len > 0.0f)
            {
                x /= len;
                y /= len;
                z /= len;
            }
        }

        // ---------------------------------------------------------------------
        // Dot and Cross Product
        // ---------------------------------------------------------------------

        // Dot Product:
        //  Measures how aligned two vectors are.
        //  Returns a scalar value (positive = same direction, negative = opposite)
        float Dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        // Cross Product:
        //  Creates a new vector that’s perpendicular to both inputs.
        //  Commonly used for surface normals or orientation axes.
        Vector3 Cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // ---------------------------------------------------------------------
        // Distance Between Two Points
        // ---------------------------------------------------------------------

        // Returns the Euclidean distance between this vector and another
        float Distance(const Vector3& other) const
        {
            float dx = x - other.x;
            float dy = y - other.y;
            float dz = z - other.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        // ---------------------------------------------------------------------
        // Linear Interpolation (Lerp)
        // ---------------------------------------------------------------------

        // Smoothly blends between vectors A and B using t (0.0 to 1.0)
        // Example: t=0 returns A, t=1 returns B, t=0.5 is halfway between.
        static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
        {
            return a + (b - a) * t;
        }

        // ---------------------------------------------------------------------
        // Clamp
        // ---------------------------------------------------------------------

        // Limits each component of a vector within a min/max range
        static Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max)
        {
            return Vector3(
                std::fmax(min.x, std::fmin(value.x, max.x)),
                std::fmax(min.y, std::fmin(value.y, max.y)),
                std::fmax(min.z, std::fmin(value.z, max.z))
            );
        }

        // ---------------------------------------------------------------------
        // Angle Between Two Vectors (radians)
        // ---------------------------------------------------------------------

        // Returns the smallest angle between two vectors in radians
        // Uses the dot product divided by magnitudes and clamps the result
        // to prevent invalid acos() input (floating-point safety).
        static float AngleBetween(const Vector3& a, const Vector3& b)
        {
            float dot = a.Dot(b);
            float lengths = a.Length() * b.Length();
            if (lengths == 0.0f) return 0.0f;

            // Clamp to [-1, 1] to prevent NaN due to floating-point drift
            float cosTheta = std::fmax(-1.0f, std::fmin(1.0f, dot / lengths));
            return std::acos(cosTheta);
        }

        // ---------------------------------------------------------------------
        // Print Helper
        // ---------------------------------------------------------------------

        // Prints the vector components to the console
        // Mainly used for debugging and quick inspections.
        void Print() const
        {
            std::cout << "Vector3(" << x << ", " << y << ", " << z << ")";
        }
    };
}

