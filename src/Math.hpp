#ifndef RBST_MATH_HPP
#define RBST_MATH_HPP

//fpm
#include <fpm/fixed.hpp>
#include <fpm/math.hpp>
//Raylib
#include <raylib.h>

using num_det = fpm::fixed<std::int32_t, std::int64_t, 16>;
using int8 = std::int8_t;
using int16 = std::int16_t;

struct Vec2
{
	num_det x;
	num_det y;
};

namespace v2
{
	inline Vec2 zero()
	{
		return Vec2{ num_det{ 0 }, num_det{ 0 } };
	}

	inline Vec2 up()
	{
		return Vec2{ num_det{ 0 }, num_det{ 1 } };
	}

	inline Vec2 down()
	{
		return Vec2{ num_det{ 0 }, num_det{ -1 } };
	}

	inline Vec2 left()
	{
		return Vec2{ num_det{ -1 }, num_det{ 0 } };
	}

	inline Vec2 right()
	{
		return Vec2{ num_det{ 1 }, num_det{ 0 } };
	}

	inline Vec2 add(Vec2 a, Vec2 b)
	{
		return Vec2{ a.x + b.x, a.y + b.y };
	}

	inline Vec2 sub(Vec2 a, Vec2 b)
	{
		return Vec2{ a.x - b.x, a.y - b.y };
	}

	inline num_det dot(Vec2 a, Vec2 b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}

	inline bool equal(Vec2 a, Vec2 b)
	{
		return (a.x == b.x) && (a.y == b.y);
	}

	inline Vec2 scalarMult(Vec2 v, num_det n)
	{
		return Vec2{ v.x * n, v.y * n };
	}

	inline Vec2 scalarDiv(Vec2 v, num_det n)
	{
		return Vec2{ v.x / n, v.y / n };
	}

	inline num_det length(Vec2 v)
	{
		return fpm::sqrt((v.x * v.x) + (v.y * v.y));
	}

	Vec2 normalize(Vec2 v)
	{
		num_det len = length(v);
		return (len == num_det{ 0 }) ? zero() : scalarDiv(v, len);
	}

	//if you want to multiply the normal by a value
	Vec2 normalizeMult(Vec2 v, num_det n)
	{
		num_det len = length(v);
		return (len == num_det{ 0 }) ? zero() : scalarMult(v, n / len);
	}

	inline Vec2 lerp(Vec2 zero, Vec2 one, num_det alpha)
	{
		return add(zero, scalarMult(sub(one, zero), alpha));
	}

	//angle in radians, please
	//positive rotates counter-clockwise
	Vec2 rotate(Vec2 v, num_det angle)
	{
		num_det cos = fpm::cos(angle);
		num_det sin = fpm::sin(angle);
		num_det x = (v.x * cos) - (v.y * sin);
		num_det y = (v.x * sin) + (v.y * cos);
		return Vec2{ x, y };
	}
	
	//projection of a on b
	Vec2 projection(Vec2 a, Vec2 b)
	{
		Vec2 b_normal = normalize(b);
		num_det relative = dot(a, b_normal);
		return scalarMult(b_normal, relative);
	}

	//rejection of a from b
	inline Vec2 rejection(Vec2 a, Vec2 b)
	{
		return sub(a, projection(a,b));
	}

	// returns <dot product of (point-origin) and ray vector, distance from closest point in ray to point>
	Vec2 closest(Vec2 rayOrig, Vec2 rayVec, Vec2 point)
	{
		Vec2 o2c = sub(point, rayOrig);
		num_det relative = dot(o2c, rayVec);
		Vec2 projection = scalarMult(rayVec, relative);
		Vec2 closest = sub(o2c, projection);
		return Vec2{ relative , length(closest) };
	}

	inline bool rayWithinRadius(num_det dot, num_det dist, num_det radius)
	{
		return (dot > num_det{ 0 }) && (dist < radius);
	}
}

inline Vector3 fromDetVec2(Vec2 vec, float height = 0.0f)
{
	return Vector3{ static_cast<float>(vec.x), height, static_cast<float>(vec.y) };
}

Vector3 fromDetVec2WithShake(Vec2 vec, Vec2 camRight, float height = 0.0f, float shakeAmount = 0.0f)
{
	Vec2 shaken = v2::add(vec, v2::scalarMult(camRight, num_det{ shakeAmount }));
	return Vector3{ static_cast<float>(shaken.x), height, static_cast<float>(shaken.y) };
}

inline float fromDetNum(num_det num)
{
	return static_cast<float>(num);
}

inline float angleFromDetVec2(Vec2 vec)
{
	return atan2(static_cast<float>(vec.y), static_cast<float>(vec.x));
}

#endif