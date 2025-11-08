#pragma once
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include "Vector2.h"
#include "Vector3.h"

namespace HiveMnd::Math
{
    // -----------------------------------------------------------------------------
    // HiveMnd Utility Library : Advanced Math Module
    // -----------------------------------------------------------------------------
    // Overview:
    //  The Advanced Math module extends HiveMnd’s math foundation with higher-level
    //  operations used across simulation, physics, AI, and procedural systems.
    //  It includes angle conversions, interpolation curves, geometric helpers,
    //  vector projection/reflection, and statistical calculations.
    //
    //  Unlike the core Vector2/Vector3 utilities, this module focuses on compound
    //  mathematical relationships and is self-contained—no dependency on operator
    //  overloads or external libraries. It is designed for performance and clarity.
    //
    // Key Features:
    //  • Safe inline math functions with explicit clamping and validation
    //  • Angle, distance, and interpolation functions for 2D/3D space
    //  • Statistical helpers (mean, variance, standard deviation)
    //  • Geometric and random generation utilities for procedural content
    //  • Cross-compatible with all HiveMnd::Math vector types
    //
    // Usage:
    //  #include "Math/AdvancedMath.h"
    //  using namespace HiveMnd::Math;
    //  float angle = ToDegrees(AngleBetween(v1, v2));
    // -----------------------------------------------------------------------------


    // Local PI constant (no <numbers> / M_PI dependency)
    inline constexpr float kPI = 3.14159265358979323846f;

    // ----- Internal helpers (no external dependencies) -----------------------
    inline float Dot2(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
    inline float Dot3(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

    inline float Length2(const Vector2& v) { return std::sqrt(Dot2(v, v)); }
    inline float Length3(const Vector3& v) { return std::sqrt(Dot3(v, v)); }

    // -------------------------------------------------------------------------
    // ANGLE OPERATIONS
    // -------------------------------------------------------------------------

    // Angle between two 3D vectors (radians)
    inline float AngleBetween(const Vector3& a, const Vector3& b)
    {
        float mags = Length3(a) * Length3(b);
        if (mags == 0.0f) return 0.0f;
        float cosTheta = std::clamp(Dot3(a, b) / mags, -1.0f, 1.0f);
        return std::acos(cosTheta);
    }

    // Angle between two 2D vectors (radians)
    inline float AngleBetween(const Vector2& a, const Vector2& b)
    {
        float mags = Length2(a) * Length2(b);
        if (mags == 0.0f) return 0.0f;
        float cosTheta = std::clamp(Dot2(a, b) / mags, -1.0f, 1.0f);
        return std::acos(cosTheta);
    }

    inline float ToDegrees(float radians) { return radians * (180.0f / kPI); }
    inline float ToRadians(float degrees) { return degrees * (kPI / 180.0f); }

    // -------------------------------------------------------------------------
    // INTERPOLATION FUNCTIONS
    // -------------------------------------------------------------------------

    // Scalar LERP
    inline float Lerp(float a, float b, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return a + (b - a) * t;
    }

    // Vector3 LERP (component-wise; no operator dependency)
    inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return Vector3{
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        };
    }

    // SmoothStep (Hermite)
    inline float SmoothStep(float edge0, float edge1, float x)
    {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    // SmootherStep (quintic)
    inline float SmootherStep(float edge0, float edge1, float x)
    {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // -------------------------------------------------------------------------
    // STATISTICS (small helpers)
    // -------------------------------------------------------------------------

    inline float Mean(float a, float b, float c) { return (a + b + c) / 3.0f; }

    inline Vector3 Mean(const Vector3& a, const Vector3& b)
    {
        return Vector3{ (a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f };
    }

    inline float Variance(float a, float b, float c)
    {
        float m = Mean(a, b, c);
        return ((a - m) * (a - m) + (b - m) * (b - m) + (c - m) * (c - m)) / 3.0f;
    }

    inline float StandardDeviation(float a, float b, float c)
    {
        return std::sqrt(Variance(a, b, c));
    }

    // -------------------------------------------------------------------------
    // GEOMETRIC UTILITIES (no operator reliance)
    // -------------------------------------------------------------------------

    // Barycentric combination p = p1 + u*(p2-p1) + v*(p3-p1)
    inline Vector3 Barycentric(const Vector3& p1, const Vector3& p2, const Vector3& p3, float u, float v)
    {
        return Vector3{
            p1.x + (p2.x - p1.x) * u + (p3.x - p1.x) * v,
            p1.y + (p2.y - p1.y) * u + (p3.y - p1.y) * v,
            p1.z + (p2.z - p1.z) * u + (p3.z - p1.z) * v
        };
    }

    // Reflect dir across normal (handles non-unit normals)
    inline Vector3 Reflect(const Vector3& dir, const Vector3& normal)
    {
        float ndotn = Dot3(normal, normal);
        if (ndotn == 0.0f) return dir; // avoid div-by-zero
        float s = 2.0f * Dot3(dir, normal) / ndotn;
        return Vector3{ dir.x - s * normal.x, dir.y - s * normal.y, dir.z - s * normal.z };
    }

    // Project a onto b (handles b=0)
    inline Vector3 Project(const Vector3& a, const Vector3& b)
    {
        float denom = Dot3(b, b);
        if (denom == 0.0f) return Vector3{ 0.0f, 0.0f, 0.0f };
        float s = Dot3(a, b) / denom;
        return Vector3{ b.x * s, b.y * s, b.z * s };
    }

    // -------------------------------------------------------------------------
    // RANDOM UTILITIES
    // -------------------------------------------------------------------------

    inline float RandomRange(float min, float max)
    {
        float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return min + t * (max - min);
    }

    // Uniform random unit vector on sphere
    inline Vector3 RandomUnitVector()
    {
        float theta = RandomRange(0.0f, 2.0f * kPI);
        float u = RandomRange(-1.0f, 1.0f);      // cos(phi) in [-1,1]
        float s = std::sqrt(std::max(0.0f, 1.0f - u * u)); // sin(phi)
        return Vector3{ s * std::cos(theta), s * std::sin(theta), u };
    }
}
