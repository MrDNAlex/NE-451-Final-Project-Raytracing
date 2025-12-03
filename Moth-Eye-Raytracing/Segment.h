#pragma once
#include <list>
#include "Vec2.h"
#include "RayHit.h"
#include "Ray.h"
#include "cmath"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Segment
{
public:

	Vec2 A;

	Vec2 B;

	std::function<double(double)> RefractiveIndexFunction;

	Segment(double x1, double y1, double x2, double y2, std::function<double(double)> refractiveIndexFunc) : A(x1, y1), B(x2, y2), RefractiveIndexFunction(refractiveIndexFunc)
	{
	}

	double GetRefractiveIndex(double wavelength)
	{
		return RefractiveIndexFunction(wavelength);
	}

	Vec2 GetNormal(bool left = true)
	{
		Vec2 dir = B - A;
		Vec2 normal = Vec2(0, 0);

		if (left)
			normal = Vec2(-dir.Y, dir.X);
		else
			normal = Vec2(dir.Y, -dir.X);

		normal.Normalize();
		return normal;
	}

	RayHit Intersect(Ray* ray)
	{
		Vec2 segment = B - A;

		double denom = ray->Direction.Cross(segment);

		if (std::abs(denom) <= EPSILON)
			// Parallel lines
			return RayHit(false, 0.0, nullptr);

		double invDenom = 1.0 / denom;
		Vec2 aToOrigin = A - ray->Origin;

		double t = aToOrigin.Cross(segment) * invDenom;
		double s = aToOrigin.Cross(ray->Direction) * invDenom;

		if (t < EPSILON || (s < 0.0 || s > 1.0))
			// No intersection
			return RayHit(false, 0.0, this);

		return RayHit(true, t, nullptr);
	}

	void Reflect(Ray* ray)
	{
		RayHit hit = Intersect(ray);

		Vec2 newOrigin = ray->GetIntersectionPosition(hit.Distance);

		Vec2 direction = ray->Direction;
		Vec2 normal = GetNormal();

		direction.Normalize();
		normal.Normalize();

		Vec2 newDirection = direction - (normal * 2.0 * direction.Dot(normal));

		ray->Origin = newOrigin;
		ray->Direction = newDirection;
	}

	void Transmit(Ray* ray)
	{
		RayHit hit = Intersect(ray);
		Vec2 newOrigin = ray->GetIntersectionPosition(hit.Distance);
		Vec2 normal = GetNormal();
		Vec2 direction = ray->Direction;

		double n1 = ray->CurrentMedium;
		double n2 = GetRefractiveIndex(ray->Wavelength);

		if (direction.Dot(normal) > 0)
			normal = normal * -1.0;

		double incidentCos = -normal.Dot(direction);
		double incidentSin = std::sqrt(std::max(0.0, 1.0 - incidentCos * incidentCos));

		double transmitSin = (n1 / n2) * incidentSin;

		if (transmitSin >= 1.0)
			// Total internal reflection
			return Reflect(ray);

		double transmissionCos = std::sqrt(std::max(0.0, 1.0 - transmitSin * transmitSin));

		Vec2 newDirection = direction * (n1 / n2) + normal * ((n1 / n2) * incidentCos - transmissionCos);
		newDirection.Normalize();

		ray->Origin = newOrigin;
		ray->Direction = newDirection;
	}

	double GetCenterX()
	{
		return 0.5 * (A.X + B.X);
	}

	double GetCenterY()
	{
		return 0.5 * (A.Y + B.Y);
	}

	json ToJSON()
	{
		json j;
		j["A"] = A.ToJSON();
		j["B"] = B.ToJSON();
		j["RefractiveIndex"] = GetRefractiveIndex(500);
		return j;
	}
};