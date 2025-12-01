#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

const double EPSILON = 1e-12;

class Vec2
{
public:

	double X;

	double Y;

	Vec2(double x, double y)
	{
		this->X = x;
		this->Y = y;
	}

	double Cross(Vec2& v2)
	{
		return X * v2.Y - Y * v2.X;
	}

	double Dot(Vec2& v2)
	{
		return X * v2.X + Y * v2.Y;
	}

	void Normalize()
	{
		double length = sqrt(X * X + Y * Y);

		if (std::abs(length) < EPSILON)
			length = EPSILON;

		X /= length;
		Y /= length;
	}

	Vec2 GetNormal(bool left = true)
	{
		if (left)
		{
			Vec2 normal = Vec2(-Y, X);
			normal.Normalize();
			return normal;
		}
		else
		{
			Vec2 normal = Vec2(Y, -X);
			normal.Normalize();
			return normal;
		}
	}

	Vec2 operator+(const Vec2& o) const
	{
		return { X + o.X, Y + o.Y };
	}

	Vec2 operator-(const Vec2& o) const
	{
		return { X - o.X, Y - o.Y };
	}

	Vec2 operator*(double s) const
	{
		return { X * s, Y * s };
	}

	Vec2 operator/(double s) const
	{
		return { X / s, Y / s };
	}

	Vec2& operator+=(const Vec2& o)
	{
		X += o.X;
		Y += o.Y;

		return *this;
	}

	Vec2& operator-=(const Vec2& o)
	{
		X -= o.X;
		Y -= o.Y;
		return *this;
	}

	Vec2& operator*=(double s)
	{
		X *= s;
		Y *= s;
		return *this;
	}

	Vec2& operator/=(double s)
	{
		X /= s;
		Y /= s;
		return *this;
	}

	json ToJSON()
	{
		json j;
		j["X"] = X;
		j["Y"] = Y;
		return j;
	}
};