/* Copyright 2019 Christian Löpke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define _USE_MATH_DEFINES  // Necessary for windows not defining M_PI by default in math.h :/
#include <math.h>
#include <limits>
#include <ClippedUtils/cString.h>

namespace Clipped
{
    template <typename T>
    inline T deg2rad(const T degrees)
    {
        return static_cast<T>((degrees * M_PI) / 180.f);
    }

    template <typename T>
    inline T rad2deg(const T rad)
    {
        return (rad * 180) / M_PI;
    }

    template <typename T>
    class Vector2
    {
    public:
        /**
         * @brief Vector2 creates a zero initialized vector.
         */
        Vector2() : Vector2(0) {}

        /**
         * @brief Vector2 creates a value initialized vector.
         * @param value to initialize all components with.
         */
        Vector2(const T value) : Vector2(value, value) {}

        /**
         * @brief Vector2 crates a value initialized vector.
         * @param x component to set.
         * @param y component to set.
         */
        Vector2(const T x, const T y) : x(x), y(y) {}

        /**
         * @brief min compares with vector and keeps min components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector2<T>& minimum(const Vector2<T>& rhs)
        {
            if (x > rhs.x) x = rhs.x;
            if (y > rhs.y) y = rhs.y;
            return *this;
        }

        /**
         * @brief max compares with vector and keeps max components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector2<T>& maximum(const Vector2<T>& rhs)
        {
            if (x < rhs.x) x = rhs.x;
            if (y < rhs.y) y = rhs.y;
            return *this;
        }

        /**
         * @brief abs returns the absolute value of this vector.
         * @return absolute vector.
         */
        inline Vector2<T> abs() const { return Vector2<T>(fabs(this->x), fabs(this->y)); }

        /**
         * @brief length returns the length of this vector.
         * @return the vector length.
         */
        inline T length() const { return sqrt((this->x * this->x) + (this->y * this->y)); }

        /**
         * @brief normalize normalizes a vector.
         * @return the normalized vector.
         */
        inline Vector2<T> normalize() const
        {
            T len = length();
            if (len == 0.0) return Vector2();
            return Vector2<T>(x / len, y / len);
        }

        /**
         * @brief operator + addition of two vectors.
         * @param rhs second summand.
         * @return a new vector containing the result.
         */
        inline Vector2<T> operator+(const Vector2<T>& rhs) const
        {
            return Vector2<T>(this->x + rhs.x, this->y + rhs.y);
        }

        /**
         * @brief operator - subtraction of two vectors.
         * @param rhs subtrahend.
         * @return a new vector containing the result.
         */
        inline Vector2<T> operator-(const Vector2<T>& rhs) const
        {
            return Vector2<T>(this->x - rhs.x, this->y - rhs.y);
        }

        /**
         * @brief operator * scalar multiplication.
         * @param rhs scalar value to muliply with.
         * @return resulting vector
         */
        inline Vector2<T> operator*(const T& rhs) const { return Vector2<T>(x * rhs, y * rhs); }

        /**
         * @brief operator == checks if all components are EXACTLY equal
         *   Note: If you don't want exact equality, use Vec.like(otherVec).
         * @param rhs compare this with rhs vector.
         * @return true if equal, false otherwise
         */
        inline bool operator==(const Vector2<T>& rhs) const
        {
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
            return (this->x == rhs.x && this->y == rhs.y);
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
        }

        /**
         * @brief operator != checks if any component isn't EXACTLY equal
         * @param rhs to compare with this.
         * @return true if unequal, false otherwise.
         */
        inline bool operator!=(const Vector2<T>& rhs) const { return !(*this == rhs); }

        /**
         * @brief like checks if this vector is almost equal rhs (epsilon range comparison)
         * @param rhs vector to compare with.
         * @return true if almost equal, false otherwise.
         */
        inline bool like(const Vector2<T>& rhs, T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ((*this - rhs).length() < epsilon);
        }

        inline bool unlike(const Vector2<T>& rhs,
                           T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return !this->like(rhs, epsilon);
        }

        T crossProduct(const Vector2<T>& a, const Vector2<T>& b) { return a.x * b.y - a.y * b.x; }

        inline String toString() const
        {
            return String("[") + String(this->x) + String(", ") + String(this->y) + String("]");
        }

        union
        {
            struct
            {
                T x;  //!< The x component of this vector.
                T y;  //!< The y component of this vector.
            };

            T v[2];  //!< Array containing all parts of this vector.
        };
    };

    //===============================================================================================

    template <typename T>
    /**
     * @brief The Vector3 class represents a vector with 3 components.
     */
    class Vector3
    {
    public:
        /**
         * @brief Vector3 creates a zero initialized vector.
         */
        Vector3() : Vector3<T>(0) {}

        /**
         * @brief Vector3 creates a vector with all elements set to value.
         * @param value to set all components to.
         */
        Vector3(const T value) : Vector3<T>(value, value, value) {}

        /**
         * @brief Vector3 creates a vector from given components.
         * @param x init component for x
         * @param y init component for y
         * @param z init component for z
         */
        Vector3(const T x, const T y, const T z) : x(x), y(y), z(z) {}

        Vector3(const Vector2<T>& vec2) : Vector3<T>(vec2.x, vec2.y, 1) {}

        /**
         * @brief min compares with vector and keeps min components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector3<T>& minimum(const Vector3<T>& rhs)
        {
            if (x > rhs.x) x = rhs.x;
            if (y > rhs.y) y = rhs.y;
            if (z > rhs.z) z = rhs.z;
            return *this;
        }

        /**
         * @brief max compares with vector and keeps max components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector3<T>& maximum(const Vector3<T>& rhs)
        {
            if (x < rhs.x) x = rhs.x;
            if (y < rhs.y) y = rhs.y;
            if (z < rhs.z) z = rhs.z;
            return *this;
        }

        /**
         * @brief abs returns the absolute value of this vector.
         * @return absolute vector.
         */
        inline Vector3<T> abs() const
        {
            return Vector3<T>(fabs(this->x), fabs(this->y), fabs(this->z));
        }

        /**
         * @brief length returns the length of this vector.
         * @return the vector length.
         */
        inline T length() const
        {
            return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
        }

        /**
         * @brief normalize normalizes a vector.
         * @return the normalized vector.
         */
        inline Vector3<T> normalize() const
        {
            T len = length();
            if (len == 0.0) return Vector3();
            return Vector3<T>(x / len, y / len, z / len);
        }

        /**
         * @brief operator + addition of two vectors.
         * @param rhs second summand.
         * @return a new vector containing the result.
         */
        inline Vector3<T> operator+(const Vector3<T>& rhs) const
        {
            return Vector3<T>(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
        }

        /**
         * @brief operator - subtraction of two vectors.
         * @param rhs subtrahend.
         * @return a new vector containing the result.
         */
        inline Vector3<T> operator-(const Vector3<T>& rhs) const
        {
            return Vector3<T>(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
        }

        /**
         * @brief operator * scalar multiplication.
         * @param rhs scalar value to muliply with.
         * @return resulting vector
         */
        inline Vector3<T> operator*(const T& rhs) const
        {
            return Vector3<T>(x * rhs, y * rhs, z * rhs);
        }

        /**
         * @brief operator == checks if all components are EXACTLY equal
         *   Note: If you don't want exact equality, use Vec.like(otherVec).
         * @param rhs compare this with rhs vector.
         * @return true if equal, false otherwise
         */
        inline bool operator==(const Vector3<T>& rhs) const
        {
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
            return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
        }

        /**
         * @brief operator != checks if any component isn't EXACTLY equal
         * @param rhs to compare with this.
         * @return true if unequal, false otherwise.
         */
        inline bool operator!=(const Vector3<T>& rhs) const { return !(*this == rhs); }

        /**
         * @brief like checks if this vector is almost equal rhs (epsilon range comparison)
         * @param rhs vector to compare with.
         * @return true if almost equal, false otherwise.
         */
        inline bool like(const Vector3<T>& rhs, T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ((*this - rhs).length() < epsilon);
        }

        inline bool unlike(const Vector3<T>& rhs,
                           T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return !this->like(rhs, epsilon);
        }

        static Vector3<T> crossProduct(const Vector3<T>& a, const Vector3<T>& b)
        {
            return Vector3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
        }

        inline String toString() const
        {
            return String("[") + String(this->x) + String(", ") + String(this->y) + String(", ") +
                   String(this->z) + String("]");
        }

        // Payload of this vector:
        union
        {
            struct
            {
                T x;  //!< The x component of this vector.
                T y;  //!< The y component of this vector.
                T z;  //!< The z component of this vector.
            };        //!< Component access.

            T v[3];  //!< Array containing all parts of this vector.
        };
    };

    //===============================================================================================

    template <typename T>
    /**
     * @brief The Vector3 class represents a vector with 3 components.
     */
    class Vector4
    {
    public:
        /**
         * @brief Vector3 creates a zero initialized vector.
         */
        Vector4() : Vector4<T>(0) {}

        /**
         * @brief Vector3 creates a vector with all elements set to value.
         * @param value to set all components to.
         */
        Vector4(const T value) : Vector4<T>(value, value, value, value) {}

        /**
         * @brief Vector3 creates a vector from given components.
         * @param x init component for x
         * @param y init component for y
         * @param z init component for z
         */
        Vector4(const T x, const T y, const T z, const T w) : x(x), y(y), z(z), w(w) {}

        Vector4(const Vector3<T>& vec3) : Vector4<T>(vec3.x, vec3.y, vec3.z, 0) {}

        /**
         * @brief min compares with vector and keeps min components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector4<T>& minimum(const Vector4<T>& rhs)
        {
            if (x > rhs.x) x = rhs.x;
            if (y > rhs.y) y = rhs.y;
            if (z > rhs.z) z = rhs.z;
            if (w > rhs.w) w = rhs.w;
            return *this;
        }

        /**
         * @brief max compares with vector and keeps max components.
         * @param rhs to compare with.
         * @return reference of this vector.
         */
        inline Vector4<T>& maximum(const Vector4<T>& rhs)
        {
            if (x < rhs.x) x = rhs.x;
            if (y < rhs.y) y = rhs.y;
            if (z < rhs.z) z = rhs.z;
            if (w < rhs.w) w = rhs.w;
            return *this;
        }

        /**
         * @brief abs returns the absolute value of this vector.
         * @return absolute vector.
         */
        inline Vector4<T> abs() const
        {
            return Vector4<T>(fabs(this->x), fabs(this->y), fabs(this->z), fabs(this->w));
        }

        /**
         * @brief length returns the length of this vector.
         * @return the vector length.
         */
        inline T length() const
        {
            return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z) +
                        (this->w * this->w));
        }

        /**
         * @brief normalize normalizes a vector.
         * @return the normalized vector.
         */
        inline Vector4<T> normalize() const
        {
            T len = length();
            if (len == 0.0) return Vector4();
            return Vector4<T>(x / len, y / len, z / len, w / len);
        }

        /**
         * @brief operator + addition of two vectors.
         * @param rhs second summand.
         * @return a new vector containing the result.
         */
        inline Vector4<T> operator+(const Vector4<T>& rhs) const
        {
            return Vector4<T>(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
        }

        /**
         * @brief operator - subtraction of two vectors.
         * @param rhs subtrahend.
         * @return a new vector containing the result.
         */
        inline Vector4<T> operator-(const Vector4<T>& rhs) const
        {
            return Vector4<T>(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
        }

        /**
         * @brief operator * scalar multiplication.
         * @param rhs scalar value to muliply with.
         * @return resulting vector
         */
        inline Vector4<T> operator*(const T& rhs) const
        {
            return Vector4<T>(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        /**
         * @brief operator == checks if all components are EXACTLY equal
         *   Note: If you don't want exact equality, use Vec.like(otherVec).
         * @param rhs compare this with rhs vector.
         * @return true if equal, false otherwise
         */
        inline bool operator==(const Vector4<T>& rhs) const
        {
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
            return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
        }

        /**
         * @brief operator != checks if any component isn't EXACTLY equal
         * @param rhs to compare with this.
         * @return true if unequal, false otherwise.
         */
        inline bool operator!=(const Vector4<T>& rhs) const { return !(*this == rhs); }

        /**
         * @brief like checks if this vector is almost equal rhs (epsilon range comparison)
         * @param rhs vector to compare with.
         * @return true if almost equal, false otherwise.
         */
        inline bool like(const Vector4<T>& rhs, T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ((*this - rhs).length() < epsilon);
        }

        inline bool unlike(const Vector4<T>& rhs,
                           T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return !this->like(rhs, epsilon);
        }

        inline String toString() const
        {
            return String("[") + String(this->x) + String(", ") + String(this->y) + String(", ") +
                   String(this->z) + String(", ") + String(this->w) + String("]");
        }

        // Payload of this vector:
        union
        {
            struct
            {
                T x;  //!< The x component of this vector.
                T y;  //!< The y component of this vector.
                T z;  //!< The z component of this vector.
                T w;  //!< The w component of this vector.
            };        //!< Component access.

            T v[4];  //!< Array containing all parts of this vector.
        };
    };

    using Vec2f = Vector2<float>;
    using Vec3f = Vector3<float>;
    using Vec4f = Vector4<float>;

    //===============================================================================================

    template <typename T>
    /**
     * @brief The Vector3 class represents a vector with 3 components.
     */
    class Matrix4
    {
    public:
        Matrix4() : row1(), row2(), row3(), row4() {}
        /**
         * @brief Matrix4 creates an identity times value.
         */
        Matrix4(const T value) : Matrix4()
        {
            row1.x = value;
            row2.y = value;
            row3.z = value;
            row4.w = value;
        }

        Matrix4(const Vector4<T>& row1, const Vector4<T>& row2, const Vector4<T>& row3,
                const Vector4<T>& row4)
            : row1(row1), row2(row2), row3(row3), row4(row4)
        {
        }

        /**
         * @brief operator * matrix vector multiplication.
         * @param rhs vector operand.
         * @return resulting vector.
         */
        Vector4<T> operator*(const Vector4<T>& rhs) const
        {
            return Vector4<T>(row1.x * rhs.x + row1.y * rhs.y + row1.z * rhs.z + row1.w * rhs.w,
                              row2.x * rhs.x + row2.y * rhs.y + row2.z * rhs.z + row2.w * rhs.w,
                              row3.x * rhs.x + row3.y * rhs.y + row3.z * rhs.z + row3.w * rhs.w,
                              row4.x * rhs.x + row4.y * rhs.y + row4.z * rhs.z + row4.w * rhs.w);
        }

        /**
         * @brief operator * matrix matrix multiplication.
         * @param rhs operand.
         * @return result of the multiplication.
         */
        Matrix4<T> operator*(const Matrix4<T>& rhs) const
        {
            Matrix4<T> ret;
            for (size_t row = 0; row < 4; row++)
            {
                for (size_t col = 0; col < 4; col++)
                {
                    ret.v[row][col] = 0;
                    for (size_t i = 0; i < 4; i++) ret.v[row][col] += v[row][i] * rhs.v[i][col];
                }
            }
            return ret;
        }

        /**
         * @brief translate builds a translation matrix from given vector.
         * @param translation vector specifying the translation.
         * @return the translation matrix.
         */
        static Matrix4<T> translate(const Vector3<T>& translation)
        {
            Matrix4<T> translationMatrix(1);
            translationMatrix.row1.w = translation.x;
            translationMatrix.row2.w = translation.y;
            translationMatrix.row3.w = translation.z;
            return translationMatrix;
        }

        /**
         * @brief scale builds a scaling matrix from given vector.
         * @param scaling the vector specifying the scaling.
         * @return the scale matrix.
         */
        static Matrix4<T> scale(const Vector3<T>& scaling)
        {
            Matrix4<T> scaleMatrix(1);
            scaleMatrix.row1.x = scaling.x;
            scaleMatrix.row2.y = scaling.y;
            scaleMatrix.row3.z = scaling.z;
            return scaleMatrix;
        }

        /**
         * @brief rotateDegrees builds a rotation matrix for degree rotation.
         * @param degrees to rotate
         * @param axis used for the rotation (don't has to be normalized).
         * @return the rotation matrix.
         */
        static Matrix4<T> rotateDegrees(T degrees, const Vector3<T>& axis)
        {
            T rads = deg2rad(degrees);
            return rotateRads(rads, axis);
        }

        /**
         * @brief rotateRads builds a rotation matrix for rad rotation.
         * @param rads to rotate.
         * @param axis used for the rotation (don't has to be normalized).
         * @return the rotation matrix.
         */
        static Matrix4<T> rotateRads(T rads, const Vector3<T>& axis)
        {
            return rotateNormRads(rads, axis.normalize());
        }

        /**
         * @brief rotateNormRads builds a rotation matrix for a rad rotation with normalized axis..
         * @param rads to rotate
         * @param axis normalized axis used for the roatation.
         * @return the roatation matrix.
         */
        static Matrix4<T> rotateNormRads(T rads, const Vector3<T>& normalizedAxis)
        {
            const Vector3<T>& a = normalizedAxis;
            T s = static_cast<T>(sin(rads));
            T c = static_cast<T>(cos(rads));
            T t = 1.0f - c;

            return Matrix4<T>(Vector4<T>(t * a.x * a.x + c, t * a.x * a.y - s * a.z,
                                         t * a.x * a.z + s * a.y, 0.0),
                              Vector4<T>(t * a.x * a.y + s * a.z, t * a.y * a.y + c,
                                         t * a.y * a.z - s * a.x, 0.0),
                              Vector4<T>(t * a.x * a.z - s * a.y, t * a.y * a.z + s * a.x,
                                         t * a.z * a.z + c, 0.0),
                              Vector4<T>(0.0, 0.0, 0.0, 1.0));
        }

        static Matrix4<T> lookAt(const Vector3<T>& viewer, const Vector3<T>& target,
                                 const Vector3<T>& upVector = Vector3<T>(0.0, 1.0, 0.0))
        {
            Vector3<T> forward = (viewer - target).normalize();
            Vector3<T> right = Vector3<T>::crossProduct(upVector.normalize(), forward);
            Vector3<T> up = Vector3<T>::crossProduct(forward, right);
            return Matrix4<T>(right, up, forward, viewer);
        }

        Vector3<T> upVector() const { return Vector3<T>(v[0][1], v[1][1], v[2][1]); }

        Vector3<T> rightVector() const { return Vector3<T>(v[0][0], v[1][0], v[2][0]); }

        Vector3<T> atVector() const { return Vector3<T>(v[0][2], v[1][2], v[2][2]); }

        String toString() const
        {
            String result;
            result += "\n";
            result += row1.toString() + "\n";
            result += row2.toString() + "\n";
            result += row3.toString() + "\n";
            result += row4.toString();
            return result;
        }

        union
        {
            struct
            {
                Vector4<T> row1;  //!< first row of this matrix.
                Vector4<T> row2;  //!< second row of this matrix.
                Vector4<T> row3;  //!< third row of this matrix.
                Vector4<T> row4;  //!< fourth row of this matrix.
            };

            T v[4][4];  //!< Array containing all vectors
        };
    };

    using Mat4f = Matrix4<float>;

    //===============================================================================================

    template <typename T>
    /**
     * @brief The Quaternion class implements calculations with quaternions.
     */
    class Quaternion
    {
    public:
        Quaternion() : Quaternion(1.f, Vector3<T>(0.f)) {}

        Quaternion(T s, const Vector3<T>& v) : s(s), v(v) {}

        Quaternion(T a, T b, T c, T d) : a(a), b(b), c(c), d(d) {}

        /**
         * @brief operator + addition of two quaternions.
         * @param rhs second summand.
         * @return resulting Quaternion.
         */
        inline Quaternion<T> operator+(const Quaternion<T>& rhs) const
        {
            return Quaternion(a + rhs.a, b + rhs.b, c + rhs.c, d + rhs.d);
        }

        /**
         * @brief operator - subtraction of two quaternions.
         * @param rhs subtrahend.
         * @return resulting Quationion.
         */
        inline Quaternion<T> operator-(const Quaternion<T>& rhs) const
        {
            return Quaternion(a - rhs.a, b - rhs.b, c - rhs.c, d - rhs.d);
        }

        /**
         * @brief operator * multiplication of two Quaternions (not cummutative!)
         * @param rhs second 'factor'.
         * @return resulting Quaternion.
         */
        inline Quaternion<T> operator*(const Quaternion<T>& rhs) const
        {
            return Quaternion(a * rhs.a - b * rhs.b - c * rhs.c - d * rhs.d,
                              a * rhs.b + b * rhs.a + c * rhs.d - d * rhs.c,
                              a * rhs.c - b * rhs.d + c * rhs.a + d * rhs.b,
                              a * rhs.d + b * rhs.c - c * rhs.b + d * rhs.a);
        }

        /**
         * @brief dot the dot product of this and rhs Quaternion.
         * @param rhs the right hand side.
         * @return the dot product.
         */
        inline T dot(const Quaternion<T>& rhs) const
        {
            return a * rhs.a + b * rhs.b + c * rhs.c + d * rhs.d;
        }

        /**
         * @brief normalize normalizes the Quaternion.
         */
        inline void normalize()
        {
            T len = static_cast<T>(1.0) / (a * a + b * b + c * c + d * d);
            a *= len;
            b *= len;
            c *= len;
            d *= len;
        }

        /**
         * @brief conj builds the conjugated form of this Quaternion.
         * @return conjugated form of Quaternion.
         */
        inline Quaternion<T> conj() const { return Quaternion<T>(a, -b, -c, -d); }

        /**
         * @brief Quaternion::rotateDeg adds a rotation to this quaternion.
         * @param degs to rotate.
         * @param axis to rotate with.
         * @return reference of this quaternion.
         */
        inline Quaternion<T>& rotateDeg(T degs, const Vector3<T>& axis)
        {
            T rads = deg2rad(degs);
            return rotateRad(rads, axis);
        }

        /** @copydoc Quaternion::rotateDeg */
        inline Quaternion<T>& rotateRad(T rads, const Vector3<T>& axis)
        {
            T tmp = sin(rads / 2.0f);
            *this = *this * Quaternion<T>(cos(rads / 2.0f), axis * tmp);
            return *this;
        }

        /**
         * @brief slerp gets interpolaten step 'progress (0.0->0.1)' from 'from' to 'to'.
         * @param from Quaternion interpolation start.
         * @param to Quaternion interpolation end.
         * @param progress desired intermediate step.
         * @return the interpolated Quaternion.
         */
        inline Quaternion<T>& slerp(const Quaternion<T>& from, const Quaternion<T>& to,
                                    float progress)
        {
            T dotproduct = from.dot(to);
            float theta, st, sut, sout, coeff1, coeff2;

            progress /= 2.0f;
            theta = static_cast<T>(acos(dotproduct));
            if (theta < 0.0f) theta = -theta;

            st = static_cast<T>(sin(theta));
            sut = static_cast<T>(sin(progress * theta));
            sout = static_cast<T>(sin((1 - progress) * theta));
            coeff1 = sout / st;
            coeff2 = sut / st;

            b = coeff1 * from.v.x + coeff2 * to.v.x;
            c = coeff1 * from.v.y + coeff2 * to.v.y;
            d = coeff1 * from.v.z + coeff2 * to.v.z;
            a = coeff1 * from.s + coeff2 * to.s;

            normalize();

            return *this;
        }

        /**
         * @brief toRotMatrix converts the Quaternion to a rotation matrix.
         *   Note: The Quaternion has to be axis normalized!
         *   Concat rot quaternions by: rot a then rot b = (rota * rotb) toRotMat.
         * @return the equivalent Matrix.
         */
        inline Matrix4<T> toRotMatrix() const
        {
            // Summarize calculations
            const float a2 = a * a;
            const float b2 = b * b;
            const float c2 = c * c;
            const float d2 = d * d;
            const float bc2 = 2.f * b * c;
            const float bd2 = 2.f * b * d;
            const float ad2 = 2.f * a * d;
            const float ab2 = 2.f * a * b;
            const float ac2 = 2.f * a * c;
            const float cd2 = 2.f * c * d;

            return Matrix4<T>(Vector4<T>(a2 + b2 - c2 - d2, bc2 - ad2, bd2 + ac2, 0.f),
                              Vector4<T>(bc2 + ad2, a2 - b2 + c2 - d2, cd2 - ab2, 0.f),
                              Vector4<T>(bd2 - ac2, cd2 + ab2, a2 - b2 - c2 + d2, 0.f),
                              Vector4<T>(0.f, 0.f, 0.f, 1.f));
        }

        // Payload of this quaternion:
        union
        {
            struct
            {
                T s;           //!< The scalar component of this quaternion.
                Vector3<T> v;  //!< The vector component of this quaternion.
            };                 //!< Component access.

            struct
            {
                T a;  //!< The scalar component.
                T b;  //!< The i component.
                T c;  //!< The j component.
                T d;  //!< The k component.
            };
        };
    };

    using Quat4f = Quaternion<float>;

}  // namespace Clipped
