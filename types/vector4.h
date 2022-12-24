#pragma once
#include <cmath>
#include <limits>
#include "vector3.h"
#define M_PI 3.14159265358979323846f

typedef float vec_t;
// 3D Vector4D
class Vector4D //-V690
{
public:
    // Members
    vec_t x, y, z, w;

    // Construction/destruction:
    Vector4D(void);
    Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W);
    Vector4D(vec_t* clr);

    // Initialization
    void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f, vec_t iw = 0.0f);
    // TODO (Ilya): Should there be an init that takes a single float for consistency?

    // Got any nasty NAN's?
    bool IsValid() const;
    void Invalidate();

    // array access...
    vec_t operator[](int i) const;
    vec_t& operator[](int i);

    // Base address...
    vec_t* Base();
    vec_t const* Base() const;

    // Initialization methods
    void Random(vec_t minVal, vec_t maxVal);
    void Zero(); ///< zero out a vector

    // equality
    bool operator==(const Vector4D& v) const;
    bool operator!=(const Vector4D& v) const;

    // arithmetic operations
    Vector4D& operator+=(const Vector4D& v)
    {
        x += v.x; y += v.y; z += v.z; w += v.w;
        return *this;
    }

    Vector4D& operator-=(const Vector4D& v)
    {
        x -= v.x; y -= v.y; z -= v.z; w -= v.w;
        return *this;
    }

    Vector4D& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        w *= fl;
        return *this;
    }

    Vector4D& operator*=(const Vector4D& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    Vector4D& operator/=(const Vector4D& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // this ought to be an opcode.
    Vector4D& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        w += fl;
        return *this;
    }

    // this ought to be an opcode.
    Vector4D& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        w /= fl;
        return *this;
    }
    Vector4D& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        w -= fl;
        return *this;
    }

    // negate the vector components
    void Negate();

    // Get the vector's magnitude.
    vec_t Length() const;

    // Get the vector's magnitude squared.
    vec_t LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }

    // return true if this vector is (0,0,0) within tolerance
    bool IsZero(float tolerance = 0.01f) const
    {
        return (x > -tolerance && x < tolerance&&
            y > -tolerance && y < tolerance&&
            z > -tolerance && z < tolerance&&
            w > -tolerance && w < tolerance);
    }

    vec_t NormalizeInPlace();
    Vector4D Normalized() const;
    bool IsLengthGreaterThan(float val) const;
    bool IsLengthLessThan(float val) const;

    // check if a vector is within the box defined by two other vectors
    bool WithinAABox(Vector4D const& boxmin, Vector4D const& boxmax);

    // Get the distance from this vector to the other one.
    vec_t DistTo(const Vector4D& vOther) const;

    // Get the distance from this vector to the other one squared.
    // NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' .  
    // may be able to tidy this up after switching to VC7
    vec_t DistToSqr(const Vector4D& vOther) const
    {
        Vector4D delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;
        delta.w = w - vOther.w;

        return delta.LengthSqr();
    }

    // Copy
    void CopyToArray(float* rgfl) const;

    // Multiply, add, and assign to this (ie: *this = a + b * scalar). This
    // is about 12% faster than the actual vector equation (because it's done per-component
    // rather than per-vector).
    void MulAdd(const Vector4D& a, const Vector4D& b, float scalar);

    // Dot product.
    vec_t Dot(const Vector4D& vOther) const;

    // assignment
    Vector4D& operator=(const Vector4D& vOther);

    // 2d
    vec_t Length2D(void) const;
    vec_t Length2DSqr(void) const;

    /// Get the component of this vector parallel to some other given vector
    Vector4D  ProjectOnto(const Vector4D& onto);

    // copy constructors
    // Vector4D(const Vector4D &vOther);

    // arithmetic operations
    Vector4D operator-(void) const;

    Vector4D operator+(const Vector4D& v) const;
    Vector4D operator-(const Vector4D& v) const;
    Vector4D operator*(const Vector4D& v) const;
    Vector4D operator/(const Vector4D& v) const;
    Vector4D operator*(float fl) const;
    Vector4D operator/(float fl) const;

    // Returns a vector with the min or max in X, Y, and Z.
    Vector4D Min(const Vector4D& vOther) const;
    Vector4D Max(const Vector4D& vOther) const;
};

void VectorCopy(const Vector4D& src, Vector4D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
}
void VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest)
{
    dest.x = src1.x + (src2.x - src1.x) * t;
    dest.y = src1.y + (src2.y - src1.y) * t;
    dest.z = src1.z + (src2.z - src1.z) * t;
    dest.w = src1.w + (src2.w - src1.w) * t;
}
float VectorLength(const Vector4D& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

vec_t NormalizeVector(Vector4D& v)
{
    vec_t l = v.Length();
    if (l != 0.0f) {
        v /= l;
    }
    else {
        v.x = v.y = v.z = v.w = 0.0f;
    }
    return l;
}

Vector4D::Vector4D(void)
{
    Invalidate();
}
Vector4D::Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}
Vector4D::Vector4D(vec_t* clr)
{
    x = clr[0];
    y = clr[1];
    z = clr[2];
    w = clr[3];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector4D::Init(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
{
    x = ix; y = iy; z = iz; w = iw;
}

void Vector4D::Random(vec_t minVal, vec_t maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    w = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
void Vector4D::Zero()
{
    x = y = z = w = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector4D& Vector4D::operator=(const Vector4D& vOther)
{
    x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w;
    return *this;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& Vector4D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector4D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
vec_t* Vector4D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector4D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector4D::IsValid() const
{
    return !isinf(x) && !isinf(y) && !isinf(z) && !isinf(w);
}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

void Vector4D::Invalidate()
{
    //#ifdef _DEBUG
    //#ifdef VECTOR_PARANOIA
    x = y = z = w = std::numeric_limits<float>::infinity();
    //#endif
    //#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector4D::operator==(const Vector4D& src) const
{
    return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool Vector4D::operator!=(const Vector4D& src) const
{
    return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
void Vector4D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; rgfl[3] = w;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------

void Vector4D::Negate()
{
    x = -x; y = -y; z = -z; w = -w;
}

// Get the component of this vector parallel to some other given vector
Vector4D Vector4D::ProjectOnto(const Vector4D& onto)
{
    return onto * (this->Dot(onto) / (onto.LengthSqr()));
}

// FIXME: Remove
// For backwards compatability
void Vector4D::MulAdd(const Vector4D& a, const Vector4D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
    z = a.z + b.z * scalar;
    w = a.w + b.w * scalar;
}

Vector4D VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t)
{
    Vector4D result;
    VectorLerp(src1, src2, t, result);
    return result;
}

vec_t Vector4D::Dot(const Vector4D& b) const
{
    return (x * b.x + y * b.y + z * b.z + w * b.w);
}
void VectorClear(Vector4D& a)
{
    a.x = a.y = a.z = a.w = 0.0f;
}

vec_t Vector4D::Length(void) const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

// check a point against a box
bool Vector4D::WithinAABox(Vector4D const& boxmin, Vector4D const& boxmax)
{
    return (
        (x >= boxmin.x) && (x <= boxmax.x) &&
        (y >= boxmin.y) && (y <= boxmax.y) &&
        (z >= boxmin.z) && (z <= boxmax.z) &&
        (w >= boxmin.w) && (w <= boxmax.w)
        );
}

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------
vec_t Vector4D::DistTo(const Vector4D& vOther) const
{
    Vector4D delta;
    delta = *this - vOther;
    return delta.Length();
}

//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------
Vector4D Vector4D::Min(const Vector4D& vOther) const
{
    return Vector4D(x < vOther.x ? x : vOther.x,
        y < vOther.y ? y : vOther.y,
        z < vOther.z ? z : vOther.z,
        w < vOther.w ? w : vOther.w);
}

Vector4D Vector4D::Max(const Vector4D& vOther) const
{
    return Vector4D(x > vOther.x ? x : vOther.x,
        y > vOther.y ? y : vOther.y,
        z > vOther.z ? z : vOther.z,
        w > vOther.w ? w : vOther.w);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector4D Vector4D::operator-(void) const
{
    return Vector4D(-x, -y, -z, -w);
}

Vector4D Vector4D::operator+(const Vector4D& v) const
{
    return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4D Vector4D::operator-(const Vector4D& v) const
{
    return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4D Vector4D::operator*(float fl) const
{
    return Vector4D(x * fl, y * fl, z * fl, w * fl);
}

Vector4D Vector4D::operator*(const Vector4D& v) const
{
    return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
}

Vector4D Vector4D::operator/(float fl) const
{
    return Vector4D(x / fl, y / fl, z / fl, w / fl);
}

Vector4D Vector4D::operator/(const Vector4D& v) const
{
    return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w);
}

Vector4D operator*(float fl, const Vector4D& v)
{
    return v * fl;
}

inline Vector quatmult(const Vector* point, Vector4D* quat)
{
    float num = quat->x * 2.f;
    float num2 = quat->y * 2.f;
    float num3 = quat->z * 2.f;
    float num4 = quat->x * num;
    float num5 = quat->y * num2;
    float num6 = quat->z * num3;
    float num7 = quat->x * num2;
    float num8 = quat->x * num3;
    float num9 = quat->y * num3;
    float num10 = quat->w * num;
    float num11 = quat->w * num2;
    float num12 = quat->w * num3;
    Vector result;
    result.x = (1.f - (num5 + num6)) * point->x + (num7 - num12) * point->y + (num8 + num11) * point->z;
    result.y = (num7 + num12) * point->x + (1.f - (num4 + num6)) * point->y + (num9 - num10) * point->z;
    result.z = (num8 - num11) * point->x + (num9 + num10) * point->y + (1.f - (num4 + num5)) * point->z;
    return result;
}
