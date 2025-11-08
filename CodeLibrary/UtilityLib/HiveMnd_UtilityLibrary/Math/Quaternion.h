#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "Vector3.h"  // Needed for rotation conversions

// -----------------------------------------------------------------------------
// HiveMnd Utility Library : Core Math Module
// File: Quaternion.h
//
// Description:
//  Represents a quaternion used for 3D rotations. Quaternions avoid the issues
//  of gimbal lock that occur with Euler angles and provide smooth, continuous
//  interpolation between orientations (slerp).
//
//  Commonly used in camera systems, physics rotations, and animation blending.
// -----------------------------------------------------------------------------

namespace HiveMnd::Math
{
    class Quaternion
    {
    public:
        float w, x, y, z; // Quaternion components

        // ---------------------------------------------------------------------
        // Constructors
        // ---------------------------------------------------------------------

        // Default constructor (identity rotation)
        Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

        // Construct from explicit component values
        Quaternion(float _w, float _x, float _y, float _z)
            : w(_w), x(_x), y(_y), z(_z) {
        }

        // Construct from axis-angle (axis normalized internally)
        Quaternion(const Vector3& axis, float angleRadians)
        {
            float halfAngle = angleRadians * 0.5f;
            float s = std::sin(halfAngle);
            Vector3 normAxis = axis.Normalized();

            w = std::cos(halfAngle);
            x = normAxis.x * s;
            y = normAxis.y * s;
            z = normAxis.z * s;
        }

        // ---------------------------------------------------------------------
        // Quaternion Normalization
        // ---------------------------------------------------------------------

        // Ensures the quaternion has unit length (important for rotation math)
        void Normalize()
        {
            float mag = std::sqrt(w * w + x * x + y * y + z * z);
            if (mag > 0.0f)
            {
                w /= mag;
                x /= mag;
                y /= mag;
                z /= mag;
            }
        }

        // Returns a normalized copy (does not modify the original)
        Quaternion Normalized() const
        {
            float mag = std::sqrt(w * w + x * x + y * y + z * z);
            if (mag > 0.0f)
                return Quaternion(w / mag, x / mag, y / mag, z / mag);
            else
                return Quaternion(1, 0, 0, 0); // fallback to identity
        }

        // ---------------------------------------------------------------------
        // Quaternion Multiplication (Rotation Combination)
        // ---------------------------------------------------------------------

        // Multiplies two quaternions together
        // This effectively combines rotations (order matters!)
        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion(
                w * q.w - x * q.x - y * q.y - z * q.z, // new w
                w * q.x + x * q.w + y * q.z - z * q.y, // new x
                w * q.y - x * q.z + y * q.w + z * q.x, // new y
                w * q.z + x * q.y - y * q.x + z * q.w  // new z
            );
        }

        // ---------------------------------------------------------------------
        // Rotate a 3D Vector by This Quaternion
        // ---------------------------------------------------------------------

        // Applies the rotation to a Vector3 using q * v * q⁻¹
        Vector3 RotateVector(const Vector3& v) const
        {
            // Represent vector as quaternion (0, v)
            Quaternion qv(0, v.x, v.y, v.z);

            // q * v * q⁻¹
            Quaternion inv = Inverse();
            Quaternion result = (*this) * qv * inv;

            return Vector3(result.x, result.y, result.z);
        }

        // ---------------------------------------------------------------------
        // Inverse / Conjugate
        // ---------------------------------------------------------------------

        // Returns the conjugate (inverse for unit quaternions)
        Quaternion Conjugate() const
        {
            return Quaternion(w, -x, -y, -z);
        }

        // Returns the inverse (useful when quaternion not unit-length)
        Quaternion Inverse() const
        {
            float normSq = w * w + x * x + y * y + z * z;
            if (normSq > 0.0f)
            {
                float invNorm = 1.0f / normSq;
                return Quaternion(w * invNorm, -x * invNorm, -y * invNorm, -z * invNorm);
            }
            return Quaternion();
        }

        // ---------------------------------------------------------------------
        // Spherical Linear Interpolation (Slerp)
        // ---------------------------------------------------------------------

        // Smoothly interpolates between two rotations (quaternions)
        // t = 0 → returns q1,  t = 1 → returns q2
        static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
        {
            // Compute cosine of angle between them
            float dot = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

            // Adjust sign to take shortest path
            Quaternion q2Copy = q2;
            if (dot < 0.0f)
            {
                dot = -dot;
                q2Copy = Quaternion(-q2.w, -q2.x, -q2.y, -q2.z);
            }

            const float DOT_THRESHOLD = 0.9995f;
            if (dot > DOT_THRESHOLD)
            {
                // Quaternions are close — use linear interpolation
                Quaternion result(
                    q1.w + t * (q2Copy.w - q1.w),
                    q1.x + t * (q2Copy.x - q1.x),
                    q1.y + t * (q2Copy.y - q1.y),
                    q1.z + t * (q2Copy.z - q1.z)
                );
                result.Normalize();
                return result;
            }

            // Perform actual slerp
            float theta0 = std::acos(dot);
            float theta = theta0 * t;
            float sinTheta = std::sin(theta);
            float sinTheta0 = std::sin(theta0);

            float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
            float s1 = sinTheta / sinTheta0;

            return Quaternion(
                (q1.w * s0) + (q2Copy.w * s1),
                (q1.x * s0) + (q2Copy.x * s1),
                (q1.y * s0) + (q2Copy.y * s1),
                (q1.z * s0) + (q2Copy.z * s1)
            );
        }

        // ---------------------------------------------------------------------
        // Conversion: Quaternion to Euler Angles (Radians)
        // ---------------------------------------------------------------------

        // Converts this quaternion to Euler pitch/yaw/roll angles
        Vector3 ToEuler() const
        {
            // roll (x-axis rotation)
            float sinr_cosp = 2 * (w * x + y * z);
            float cosr_cosp = 1 - 2 * (x * x + y * y);
            float roll = std::atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            float sinp = 2 * (w * y - z * x);
            float pitch = std::abs(sinp) >= 1 ? std::copysign(M_PI / 2, sinp) : std::asin(sinp);

            // yaw (z-axis rotation)
            float siny_cosp = 2 * (w * z + x * y);
            float cosy_cosp = 1 - 2 * (y * y + z * z);
            float yaw = std::atan2(siny_cosp, cosy_cosp);

            return Vector3(roll, pitch, yaw);
        }

        // ---------------------------------------------------------------------
        // Debug Print Helper
        // ---------------------------------------------------------------------

        // Prints the quaternion’s components for inspection
        void Print() const
        {
            std::cout << "Quaternion(" << w << ", " << x << ", " << y << ", " << z << ")";
        }
    };
}

