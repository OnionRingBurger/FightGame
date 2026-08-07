#pragma once

#include <vector>
#include <array>
#include <algorithm>

struct float3;
struct float2;

#define PI (3.1415f)
#define RAD (PI / 180.0f)
#define DEG (180.0f / PI)
#define TOFLOAT3(x) (reinterpret_cast<const float3&>(x))

struct float3{
	float x;
	float y;
	float z;

	constexpr float3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
	}

	constexpr float3(const float& a_x, const float& a_y, const float& a_z)
		: x(a_x)
		, y(a_y)
		, z(a_z)
	{
	}


	float3 operator+(const float3& num) const
	{
		float3 ret = *this;
		ret.x += num.x;
		ret.y += num.y;
		ret.z += num.z;

		return ret;
	}

	float3 operator+(const float& num) const
	{
		float3 ret = *this;
		ret.x += num;
		ret.y += num;
		ret.z += num;

		return ret;
	}

	float3 operator+=(const float3& num)
	{
		*this = *this + num;
		return *this;
	}

	float3 operator+=(const float num)
	{
		*this = *this + num;
		return *this;
	}

	float3 operator-(const float3& num) const
	{
		float3 ret = *this;
		ret.x -= num.x;
		ret.y -= num.y;
		ret.z -= num.z;

		return ret;
	}

	float3 operator-(const float& num) const
	{
		float3 ret = *this;
		ret.x -= num;
		ret.y -= num;
		ret.z -= num;

		return ret;
	}

	float3 operator-=(const float3& num)
	{
		*this = *this - num;
		return *this;
	}

	float3 operator-=(const float num)
	{
		*this = *this - num;
		return *this;
	}

	float3 operator*(const float& num) const
	{
		float3 ret = *this;
		ret.x *= num;
		ret.y *= num;
		ret.z *= num;

		return ret;
	}

	float3 operator/(const float& num) const
	{
		float3 ret = *this;
		ret.x /= num;
		ret.y /= num;
		ret.z /= num;

		return ret;
	}

	float3 operator=(const float& num)
	{
		this->x = num;
		this->y = num;
		this->z = num;

		return *this;
	}

	float3 operator=(const float3& num)
	{
		this->x = num.x;
		this->y = num.y;
		this->z = num.z;

		return *this;
	}
};

struct float2 {
	float x;
	float y;

	constexpr float2()
		: x(0.0f)
		, y(0.0f)
	{
	}

	constexpr float2(float a_x, float a_y)
		: x(a_x)
		, y(a_y)
	{
	}


	float2 operator+(const float2& num) const
	{
		float2 ret = *this;
		ret.x += num.x;
		ret.y += num.y;

		return ret;
	}

	float2 operator+(const float& num) const
	{
		float2 ret = *this;
		ret.x += num;
		ret.y += num;

		return ret;
	}

	float2 operator+=(const float2& num)
	{
		*this = *this + num;
		return *this;
	}

	float2 operator+=(const float num)
	{
		*this = *this + num;
		return *this;
	}

	float2 operator-(const float2& num) const
	{
		float2 ret = *this;
		ret.x += num.x;
		ret.y += num.y;

		return ret;
	}

	float2 operator-(const float& num) const
	{
		float2 ret = *this;
		ret.x += num;
		ret.y += num;

		return ret;
	}

	float2 operator-=(const float2& num)
	{
		*this = *this - num;
		return *this;
	}

	float2 operator-=(const float num)
	{
		*this = *this - num;
		return *this;
	}

	float2 operator*(const float& num) const
	{
		float2 ret = *this;
		ret.x *= num;
		ret.y *= num;

		return ret;
	}

	float2 operator/(const float& num) const
	{
		float2 ret = *this;
		ret.x /= num;
		ret.y /= num;

		return ret;
	}

	float2 operator=(const float& num)
	{
		this->x = num;
		this->y = num;

		return *this;
	}

	float2 operator=(const float2& num)
	{
		this->x = num.x;
		this->y = num.y;

		return *this;
	}
};



struct int3 {
	int x;
	int y;
	int z;

	constexpr int3()
		: x(0)
		, y(0)
		, z(0)
	{
	}

	constexpr int3(const int& a_x, const int& a_y, const int& a_z)
		: x(a_x)
		, y(a_y)
		, z(a_z)
	{
	}


	int3 operator+(const int3& num) const
	{
		int3 ret = *this;
		ret.x += num.x;
		ret.y += num.y;
		ret.z += num.z;

		return ret;
	}

	int3 operator+(const int& num) const
	{
		int3 ret = *this;
		ret.x += num;
		ret.y += num;
		ret.z += num;

		return ret;
	}

	int3 operator+=(const int3& num)
	{
		*this = *this + num;
		return *this;
	}

	int3 operator+=(const int num)
	{
		*this = *this + num;
		return *this;
	}

	int3 operator-(const int3& num) const
	{
		int3 ret = *this;
		ret.x -= num.x;
		ret.y -= num.y;
		ret.z -= num.z;

		return ret;
	}

	int3 operator-=(const int3& num)
	{
		*this = *this - num;
		return *this;
	}


	int3 operator-=(const int num)
	{
		*this = *this - num;
		return *this;
	}

	int3 operator-(const int& num) const
	{
		int3 ret = *this;
		ret.x -= num;
		ret.y -= num;
		ret.z -= num;

		return ret;
	}

	int3 operator*(const int& num) const
	{
		int3 ret = *this;
		ret.x *= num;
		ret.y *= num;
		ret.z *= num;

		return ret;
	}

	int3 operator/(const int& num) const
	{
		int3 ret = *this;
		ret.x /= num;
		ret.y /= num;
		ret.z /= num;

		return ret;
	}

	int3 operator=(const int& num)
	{
		this->x = num;
		this->y = num;
		this->z = num;

		return *this;
	}

	int3 operator=(const int3& num)
	{
		this->x = num.x;
		this->y = num.y;
		this->z = num.z;

		return *this;
	}
};


struct int2 {
	int x;
	int y;

	constexpr int2()
		: x(0)
		, y(0)
	{
	}

	constexpr int2(int a_x, int a_y)
		: x(a_x)
		, y(a_y)
	{
	}


	int2 operator+(const int2& num) const
	{
		int2 ret = *this;
		ret.x += num.x;
		ret.y += num.y;

		return ret;
	}

	int2 operator+(const int& num) const
	{
		int2 ret = *this;
		ret.x += num;
		ret.y += num;

		return ret;
	}

	int2 operator+=(const int2& num)
	{
		*this = *this + num;
		return *this;
	}

	int2 operator+=(const int num)
	{
		*this = *this + num;
		return *this;
	}

	int2 operator-(const int2& num) const
	{
		int2 ret = *this;
		ret.x += num.x;
		ret.y += num.y;

		return ret;
	}

	int2 operator-(const int& num) const
	{
		int2 ret = *this;
		ret.x += num;
		ret.y += num;

		return ret;
	}

	int2 operator-=(const int2& num)
	{
		*this = *this - num;
		return *this;
	}

	int2 operator-=(const int num)
	{
		*this = *this - num;
		return *this;
	}

	int2 operator*(const int& num) const
	{
		int2 ret = *this;
		ret.x *= num;
		ret.y *= num;

		return ret;
	}

	int2 operator/(const int& num) const
	{
		int2 ret = *this;
		ret.x /= num;
		ret.y /= num;

		return ret;
	}

	int2 operator=(const int& num)
	{
		this->x = num;
		this->y = num;

		return *this;
	}

	int2 operator=(const int2& num)
	{
		this->x = num.x;
		this->y = num.y;

		return *this;
	}
};


using Matrix3X3 = std::array<std::array<float, 3>, 3>;
Matrix3X3 CollectMatrix(std::vector<float3> a_vertex);
float DotFloat3(float3 a_vecA, float3 a_vecB);
Matrix3X3 GetEigenVectors(const Matrix3X3&);
float GetMaxValue(const Matrix3X3& matrix, int& p, int& q);
float3 MultiplyVector(float3, float3, float3);
float3 Normalize(const float3& a_vector);
float2 Normalize(const float2& a_vector);
float NormalizeAngle(float radAngle);
// ìxêîñ@ÇÃäpìxç∑Ç -180..180 Ç…èÙÇﬁ
float DeltaDeg(float a_fromDeg, float a_toDeg);
float3 Cross(float3, float3);
float RSqrt(float x);
float NormalizeLength(const float x, const float y, const float z);
float Sign(float num);
float3 Sign(float3 num);
float3 ExponentialRad(const float3& currentRad, const float3& targetRad, float speed, float dt);
float3 GetForward(float3 rotation);
float GetLength(float3 thisPos, float3 otherPos);
float GetLength(const float2& a_vector);
float GetLength(float2 a_thisPos, float2 a_otherPos);
float GetLengthSq(const float2& a_vector);
float2 AngleToVector(float a_angle);

float Frac(float a_value);
float Noise(float a_time);
float Lerp(float start, float end, float rate);