#pragma once
#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/mathlib/vector.h

// used: isfinite, fmodf, sqrtf
#include <cmath>
// used: numeric_limits
#include <limits>
// used:: number
#include <numbers>

class vector2d
{
public:
	constexpr vector2d(float x = 0.f, float y = 0.f) :
		x(x), y(y) { }

	[[nodiscard]] bool iszero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO);
	}

public:
	float x, y;
};

class vector
{
public:
	constexpr vector(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr vector(const float* arrvector) :
		x(arrvector[0]), y(arrvector[1]), z(arrvector[2]) { }

	constexpr vector(const vector2d& vecBase2D) :
		x(vecBase2D.x), y(vecBase2D.y), z(0.0f) { }

	[[nodiscard]] bool isvalid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}

	constexpr void invalidate()
	{
		this->x = this->y = this->z = std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float* data()
	{
		return reinterpret_cast<float*>(this);
	}

	[[nodiscard]] const float* data() const
	{
		return reinterpret_cast<float*>(const_cast<vector*>(this));
	}

	float& operator[](const std::size_t i)
	{
		return this->data()[i];
	}

	const float& operator[](const std::size_t i) const
	{
		return this->data()[i];
	}

	bool operator==(const vector& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const vector& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr vector& operator=(const vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z;
		return *this;
	}

	constexpr vector& operator=(const vector2d& vecBase2D)
	{
		this->x = vecBase2D.x; this->y = vecBase2D.y; this->z = 0.0f;
		return *this;
	}

	constexpr vector& operator+=(const vector& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y; this->z += vecBase.z;
		return *this;
	}

	constexpr vector& operator-=(const vector& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y; this->z -= vecBase.z;
		return *this;
	}

	constexpr vector& operator*=(const vector& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y; this->z *= vecBase.z;
		return *this;
	}

	constexpr vector& operator/=(const vector& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y; this->z /= vecBase.z;
		return *this;
	}

	constexpr vector& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr vector& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr vector& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr vector& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	vector operator+(const vector& vecAdd) const
	{
		return vector(this->x + vecAdd.x, this->y + vecAdd.y, this->z + vecAdd.z);
	}

	vector operator-(const vector& vecSubtract) const
	{
		return vector(this->x - vecSubtract.x, this->y - vecSubtract.y, this->z - vecSubtract.z);
	}

	vector operator*(const vector& vecMultiply) const
	{
		return vector(this->x * vecMultiply.x, this->y * vecMultiply.y, this->z * vecMultiply.z);
	}

	vector operator/(const vector& vecDivide) const
	{
		return vector(this->x / vecDivide.x, this->y / vecDivide.y, this->z / vecDivide.z);
	}

	vector operator+(const float flAdd) const
	{
		return vector(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	vector operator-(const float flSubtract) const
	{
		return vector(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	vector operator*(const float flMultiply) const
	{
		return vector(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	vector operator/(const float flDivide) const
	{
		return vector(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	[[nodiscard]] bool IsEqual(const vector& vecEqual) const
	{
		return (std::fabsf(this->x - vecEqual.x) < std::numeric_limits<float>::epsilon() &&
			std::fabsf(this->y - vecEqual.y) < std::numeric_limits<float>::epsilon() &&
			std::fabsf(this->z - vecEqual.z) < std::numeric_limits<float>::epsilon());
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO &&
			std::fpclassify(this->z) == FP_ZERO);
	}

	[[nodiscard]] vector2d Tovector2D() const
	{
		return vector2d(this->x, this->y);
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrtf(this->LengthSqr());
	}

	[[nodiscard]] constexpr float LengthSqr() const
	{
		return DotProduct(*this);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(this->Length2DSqr());
	}

	[[nodiscard]] constexpr float Length2DSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	[[nodiscard]] float DistTo(const vector& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	[[nodiscard]] constexpr float DistToSqr(const vector& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	[[nodiscard]] vector Normalized() const
	{
		vector vecOut = *this;
		vecOut.NormalizeInPlace();
		return vecOut;
	}

	float NormalizeInPlace()
	{
		const float flLength = this->Length();
		const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

		this->x *= flRadius;
		this->y *= flRadius;
		this->z *= flRadius;

		return flLength;
	}

	[[nodiscard]] constexpr float DotProduct(const vector& vecDot) const
	{
		return (this->x * vecDot.x + this->y * vecDot.y + this->z * vecDot.z);
	}

	[[nodiscard]] constexpr vector CrossProduct(const vector& vecCross) const
	{
		return vector(this->y * vecCross.z - this->z * vecCross.y, this->z * vecCross.x - this->x * vecCross.z, this->x * vecCross.y - this->y * vecCross.x);
	}

public:
	float x, y, z;
};

class vector4d
{
public:
	constexpr vector4d(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :
		x(x), y(y), z(z), w(w) { }

public:
	float x, y, z, w;
};

class __declspec(align(16)) vectorAligned : public vector
{
public:
	vectorAligned() = default;

	explicit vectorAligned(const vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
	}

	constexpr vectorAligned& operator=(const vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
		return *this;
	}

public:
	float w;
};