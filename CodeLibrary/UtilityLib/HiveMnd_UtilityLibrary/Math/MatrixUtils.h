#pragma once
#include <cmath>
#include <iostream>

namespace HiveMnd::Math
{
    // -------------------------------------------------------------------------
    // HiveMnd Utility Library : Core Math Module
    // File: Vector3.h
    //
    // Description:
    //  Represents a 3D vector and provides basic and advanced operations
    //  (addition, subtraction, scaling, dot, cross, normalization, etc.).
    //  This version uses static Cross() and Dot() for consistency across
    //  the math library.
    // -------------------------------------------------------------------------

    class Vector3
    {
    public:
        float x, y, z;

        // ---------------------------------------------------------------------
        // Constructors
        // ---------------------------------------------------------------------

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

        // ---------------------------------------------------------------------
        // Basic Arithmetic Operations
        // ---------------------------------------------------------------------

        Vector3 operator+(const Vector3& rhs) const
        {
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vector3 operator-(const Vector3& rhs) const
        {
            return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        Vector3 operator*(float scalar) const
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(float scalar) const
        {
            if (scalar == 0.0f) return Vector3(0, 0, 0);
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        Vector3& operator+=(const Vector3& rhs)
        {
            x += rhs.x; y += rhs.y; z += rhs.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs)
        {
            x -= rhs.x; y -= rhs.y; z -= rhs.z;
            return *this;
        }

        // ---------------------------------------------------------------------
        // Magnitude and Normalization
        // ---------------------------------------------------------------------

        float Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vector3 Normalized() const
        {
            float mag = Magnitude();
            return (mag == 0.0f) ? Vector3(0, 0, 0) : Vector3(x / mag, y / mag, z / mag);
        }

        // ---------------------------------------------------------------------
        // Dot and Cross Product (Static)
        // ---------------------------------------------------------------------

        //  Computes the dot product of two vectors
        static float Dot(const Vector3& a, const Vector3& b)
        {
            return (a.x * b.x + a.y * b.y + a.z * b.z);
        }

        //  Computes the cross product of two vectors
        static Vector3 Cross(const Vector3& a, const Vector3& b)
        {
            return Vector3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }

        // ---------------------------------------------------------------------
        // Utility Methods
        // ---------------------------------------------------------------------

        float DistanceTo(const Vector3& other) const
        {
            return std::sqrt(
                (x - other.x) * (x - other.x) +
                (y - other.y) * (y - other.y) +
                (z - other.z) * (z - other.z)
            );
        }

        void Print() const
        {
            std::cout << "Vector3(" << x << ", " << y << ", " << z << ")\n";
        }

        // ---------------------------------------------------------------------
        // Static Helpers
        // ---------------------------------------------------------------------

        static Vector3 Zero() { return Vector3(0, 0, 0); }
        static Vector3 One() { return Vector3(1, 1, 1); }
        static Vector3 Up() { return Vector3(0, 1, 0); }
        static Vector3 Right() { return Vector3(1, 0, 0); }
        static Vector3 Forward() { return Vector3(0, 0, 1); }
    };
}
