#pragma once
#include "Segment.h"
#include <vector>
#include <random>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct FresnelCoeffs
{
	double Reflectance;
	double Transmittance;
};

class Object
{
public:

	std::vector<Segment> Segments;

	std::string Type;

	Object()
	{
		Segments = std::vector<Segment>();
		Type = "Object";
	}

	void AddSegment(double x1, double y1, double x2, double y2, double refractiveIndex)
	{
		Segments.emplace_back(x1, y1, x2, y2, refractiveIndex);
	}

	RayHit Intersect(Ray* ray)
	{
		double minT = INFINITY;
		Segment* closestSegment = nullptr;

		for (Segment& segment : Segments)
		{
			RayHit hit = segment.Intersect(ray);

			if (hit.Hit && hit.Distance < minT)
			{
				minT = hit.Distance;
				closestSegment = &segment;
			}
		}

		if (closestSegment == nullptr)
			return RayHit(false, 0.0, nullptr);
		else
			return RayHit(true, minT, closestSegment);
	}

	virtual std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray)
	{
		FresnelCoeffs fresnel = GetFresnelCoefficients(segment, ray);

		Ray cloneRay = ray->Clone();

		std::vector<Ray> resultingRays;

		ray->Power *= fresnel.Reflectance;
		cloneRay.Power *= fresnel.Transmittance;

		segment->Reflect(ray);
		resultingRays.push_back(*ray);

		segment->Transmit(&cloneRay);
		cloneRay.CurrentMedium = segment->RefractiveIndex;
		cloneRay.CurrentBounce = 0;
		resultingRays.push_back(cloneRay);

		return resultingRays;
	}

	std::vector<double> linspace(double start, double end, int num) {
		std::vector<double> result;
		if (num <= 0) return result;
		if (num == 1) {
			result.push_back(start);
			return result;
		}

		double step = (end - start) / (num - 1);
		for (int i = 0; i < num; ++i)
			result.push_back(start + step * i);

		return result;
	}

	virtual json ToJSON()
	{
		json j;
		j["Type"] = Type;
		j["SegmentCount"] = Segments.size();
		j["Segments"] = json::array();

		for (Segment& segment : Segments)
			j["Segments"].push_back(segment.ToJSON());

		return j;
	}

protected:
	FresnelCoeffs GetFresnelCoefficients(Segment* segment, Ray* ray)
	{
		double n1 = ray->CurrentMedium;
		double n2 = segment->RefractiveIndex;

		Vec2 normal = segment->GetNormal();

		normal.Normalize();
		ray->Direction.Normalize();

		double incidentCos = -normal.Dot(ray->Direction);

		if (incidentCos < 0)
		{
			normal = normal * -1;
			incidentCos = -incidentCos;
		}

		double incidentSin = std::sqrt(std::max(0.0, 1.0 - incidentCos * incidentCos));

		double transmitSin = (n1 / n2) * incidentSin;

		if (transmitSin >= 1.0)
			return FresnelCoeffs{ 1.0, 0.0 };

		double transmitCos = std::sqrt(std::max(0.0, 1.0 - transmitSin * transmitSin));

		double n1ICos = n1 * incidentCos;
		double n2TCos = n2 * transmitCos;

		double n1TCos = n1 * transmitCos;
		double n2ICos = n2 * incidentCos;

		double denomS = n1ICos + n2TCos;
		double denomP = n2ICos + n1TCos;

		if (std::abs(denomS) < EPSILON)
			if (denomS > 0)
				denomS = EPSILON;
			else
				denomS = -EPSILON;

		if (std::abs(denomP) < EPSILON)
			if (denomP > 0)
				denomP = EPSILON;
			else
				denomP = -EPSILON;

		double inverseS = 1.0 / denomS;
		double inverseP = 1.0 / denomP;

		double rs = (n1ICos - n2TCos) * inverseS;
		double ts = (2 * n1ICos) * inverseS;

		double rp = (n2ICos - n1TCos) * inverseP;
		double tp = (2 * n1ICos) * inverseP;

		double Rs = rs * rs;
		double Rp = rp * rp;

		double R = 0.5 * (Rs + Rp);
		double T = 1 - R;

		if (R > 1.0)
			std::cout << "Warning: Reflectance > 1.0 :" << R << std::endl;

		if (T < 0.0)
			std::cout << "Warning: Transmittance < 0.0 : " << T << std::endl;

		return FresnelCoeffs{ R, T };
	}

	int randomInt(int min, int max) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(min, max);
		return dist(gen);
	}
};