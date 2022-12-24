#pragma once

#include <cmath>

#include "vector3.h"

typedef float vec_t;
// 2D Vector
class Vector2D
{
public:
    // Members
    vec_t x, y;

    // Construction/destruction:
    Vector2D(void);
    Vector2D(vec_t X, vec_t Y);
    Vector2D(vec_t* clr);

    Vector2D(const Vector2D& vOther)
    {
        x = vOther.x; y = vOther.y;
    }

    // Initialization
    void Init(vec_t ix = 0.0f, vec_t iy = 0.0f);
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
    bool operator==(const Vector2D& v) const;
    bool operator!=(const Vector2D& v) const;

    // arithmetic operations
    Vector2D& operator+=(const Vector2D& v)
    {
        x += v.x; y += v.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& v)
    {
        x -= v.x; y -= v.y;
        return *this;
    }

    Vector2D& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        return *this;
    }

    Vector2D& operator*=(const Vector2D& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vector2D& operator/=(const Vector2D& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    // this ought to be an opcode.
    Vector2D& operator+=(float fl)
    {
        x += fl;
        y += fl;
        return *this;
    }

    // this ought to be an opcode.
    Vector2D& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        return *this;
    }
    Vector2D& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        return *this;
    }

    // negate the vector components
    void Negate();

    // Get the vector's magnitude.
    vec_t Length() const;

    // Get the vector's magnitude squared.
    vec_t LengthSqr(void) const
    {
        return (x * x + y * y);
    }

    // return true if this vector is (0,0,0) within tolerance
    bool IsZero(float tolerance = 0.01f) const
    {
        return (x > -tolerance && x < tolerance&&
            y > -tolerance && y < tolerance);
    }

    vec_t NormalizeInPlace();
    Vector2D Normalized() const;
    bool IsLengthGreaterThan(float val) const;
    bool IsLengthLessThan(float val) const;

    // check if a vector is within the box defined by two other vectors
    bool WithinAABox(Vector2D const& boxmin, Vector2D const& boxmax);

    // Get the distance from this vector to the other one.
    vec_t DistTo(const Vector2D& vOther) const;

    float distance(Vector2D b)
    {
        return sqrt(pow(b.x - x, 2) + pow(b.y - y, 2));
    }

    void normalize()
    {

        if (x < -89)
            x = -89;

        else if (x > 89)
            x = 89;

        if (y < -360)
            y += 360;

        else if (y > 360)
            y -= 360;

    }

    auto clamp()
    {

        while (this->y < -89.0f)
            this->y += 89.0f;

        if (this->y > 89.0f)
            this->y = 89.0f;

        while (this->x < -180.0f)
            this->x += 360.0f;

        while (this->x > 180.0f)
            this->x -= 360.0f;

    }

    // Get the distance from this vector to the other one squared.
    // NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' .  
    // may be able to tidy this up after switching to VC7
    vec_t DistToSqr(const Vector2D& vOther) const
    {
        Vector2D delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;

        return delta.LengthSqr();
    }

    // Copy
    void CopyToArray(float* rgfl) const;

    // Multiply, add, and assign to this (ie: *this = a + b * scalar). This
    // is about 12% faster than the actual vector equation (because it's done per-component
    // rather than per-vector).
    void MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

    // Dot product.
    vec_t Dot(const Vector2D& vOther) const;

    // assignment
    Vector2D& operator=(const Vector2D& vOther);

    // 2d
    vec_t Length2D(void) const;
    vec_t Length2DSqr(void) const;
    vec_t LengthDrop() const;
    Vector2D NormalizedDrop() const
    {
        Vector2D reses = *this;
        float l = reses.LengthDrop();
        if (l != 0.0f)  //-V550
            reses /= l;
        else
            reses.x = reses.y = 0.0f;
        return reses;
    }
    vec_t DistToDrop(const Vector2D& vOther) const
    {
        Vector2D deltar;

        deltar.x = x - vOther.x;
        deltar.y = y - vOther.y;

        return deltar.LengthDrop();
    }

    /// Get the component of this vector parallel to some other given vector
    Vector2D  ProjectOnto(const Vector2D& onto);

    // copy constructors
    // Vector2D(const Vector2D &vOther);

    // arithmetic operations
    Vector2D operator-(void) const;

    Vector2D operator+(const Vector2D& v) const;
    Vector2D operator-(const Vector2D& v) const;
    Vector2D operator*(const Vector2D& v) const;
    Vector2D operator/(const Vector2D& v) const;
    Vector2D operator*(float fl) const;
    Vector2D operator/(float fl) const;

    // Cross product between two vectors.
    Vector2D Cross(const Vector2D& vOther) const;

    // Returns a vector with the min or max in X, Y, and Z.
    Vector2D Min(const Vector2D& vOther) const;
    Vector2D Max(const Vector2D& vOther) const;
};

Vector2D::Vector2D(void)
{
    x = y = 0.0f;
}

Vector2D::Vector2D(vec_t X, vec_t Y)
{
    x = X; y = Y;
}

Vector2D::Vector2D(vec_t* clr)
{
    x = clr[0]; y = clr[1];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector2D::Init(vec_t ix, vec_t iy)
{
    x = ix; y = iy;
}

void Vector2D::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

vec_t Vector2DLengthDrop(const Vector2D& v)
{
    return (vec_t)sqrt(v.x * v.x + v.y * v.y);
}

vec_t Vector2D::LengthDrop(void) const
{
    return Vector2DLengthDrop(*this);
}


void Vector2DClear(Vector2D& a)
{
    a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector2D& Vector2D::operator=(const Vector2D& vOther)
{
    x = vOther.x; y = vOther.y;
    return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

vec_t& Vector2D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector2D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

vec_t* Vector2D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector2D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector2D::IsValid() const
{
    return !isinf(x) && !isinf(y);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector2D::operator==(const Vector2D& src) const
{
    return (src.x == x) && (src.y == y);
}

bool Vector2D::operator!=(const Vector2D& src) const
{
    return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
}

void Vector2D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------

void Vector2D::Negate()
{
    x = -x; y = -y;
}

void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x + b.x;
    c.y = a.y + b.y;
}

void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x - b.x;
    c.y = a.y - b.y;
}

void Vector2DMultiply(const Vector2D& a, vec_t b, Vector2D& c)
{
    c.x = a.x * b;
    c.y = a.y * b;
}

void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x * b.x;
    c.y = a.y * b.y;
}


void Vector2DDivide(const Vector2D& a, vec_t b, Vector2D& c)
{
    vec_t oob = 1.0f / b;
    c.x = a.x * oob;
    c.y = a.y * oob;
}

void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x / b.x;
    c.y = a.y / b.y;
}

void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
    result.x = start.x + s * dir.x;
    result.y = start.y + s * dir.y;
}

// FIXME: Remove
// For backwards compatability
void Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
}

void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest)
{
    dest[0] = src1[0] + (src2[0] - src1[0]) * t;
    dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
vec_t DotProduct2D(const Vector2D& a, const Vector2D& b)
{
    return(a.x * b.x + a.y * b.y);
}

// for backwards compatability
vec_t Vector2D::Dot(const Vector2D& vOther) const
{
    return DotProduct2D(*this, vOther);
}

vec_t Vector2DNormalize(Vector2D& v)
{
    vec_t l = v.Length();
    if (l != 0.0f) {
        v /= l;
    }
    else {
        v.x = v.y = 0.0f;
    }
    return l;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
vec_t Vector2DLength(const Vector2D& v) //-V524
{
    return (vec_t)sqrt(v.x * v.x + v.y * v.y);
}

vec_t Vector2D::NormalizeInPlace()
{
    return Vector2DNormalize(*this);
}

bool Vector2D::IsLengthGreaterThan(float val) const
{
    return LengthSqr() > val * val;
}

bool Vector2D::IsLengthLessThan(float val) const
{
    return LengthSqr() < val * val;
}

vec_t Vector2D::Length(void) const
{
    return Vector2DLength(*this);
}


void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
    result.x = (a.x < b.x) ? a.x : b.x;
    result.y = (a.y < b.y) ? a.y : b.y;
}


void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
    result.x = (a.x > b.x) ? a.x : b.x;
    result.y = (a.y > b.y) ? a.y : b.y;
}

//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D* pResult)
{
    Vector2D vecDelta;
    Vector2DSubtract(vecTarget, vecStart, vecDelta);
    float flDistSqr = vecDelta.LengthSqr();
    if (flDistSqr <= flMaxDist * flMaxDist) {
        *pResult = vecTarget;
    }
    else {
        vecDelta /= sqrt(flDistSqr);
        Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
    }
}

//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

Vector2D Vector2D::Min(const Vector2D& vOther) const
{
    return Vector2D(x < vOther.x ? x : vOther.x, y < vOther.y ? y : vOther.y);
}

Vector2D Vector2D::Max(const Vector2D& vOther) const
{
    return Vector2D(x > vOther.x ? x : vOther.x, y > vOther.y ? y : vOther.y);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector2D Vector2D::operator-(void) const
{
    return Vector2D(-x, -y);
}

Vector2D Vector2D::operator+(const Vector2D& v) const
{
    Vector2D res;
    Vector2DAdd(*this, v, res);
    return res;
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
    Vector2D res;
    Vector2DSubtract(*this, v, res);
    return res;
}

Vector2D Vector2D::operator*(float fl) const
{
    Vector2D res;
    Vector2DMultiply(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator*(const Vector2D& v) const
{
    Vector2D res;
    Vector2DMultiply(*this, v, res);
    return res;
}

Vector2D Vector2D::operator/(float fl) const
{
    Vector2D res;
    Vector2DDivide(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator/(const Vector2D& v) const
{
    Vector2D res;
    Vector2DDivide(*this, v, res);
    return res;
}

Vector2D operator*(float fl, const Vector2D& v)
{
    return v * fl;
}