#pragma once
#include "RaySource.h"
class ConeLight : public RaySource
{
public:

	Vec2 Origin;

	Vec2 A;

	Vec2 B;

	ConeLight(double ox, double oy, double x1, double y1, double x2, double y2, int numberOfRays, WavelengthGenerator* wavelengthGenerator, double currentMedium = 1.0) : RaySource(numberOfRays, wavelengthGenerator, currentMedium), Origin(ox, oy), A(x1, y1), B(x2, y2)
	{
	}

	std::vector<Ray> GenerateRays() override
	{
		std::vector<Ray> rays = std::vector<Ray>();

		Vec2 AB = B - A;
		bool degenerate = (std::abs(AB.X) < 1e-12 && std::abs(AB.Y) < 1e-12);

		WavelengthGenerator& wg = *this->WavelengthGen;

		for (int i = 0; i < NumberOfRays; ++i)
		{
			double t = (double)i / (double)(NumberOfRays - 1);

			Vec2 target = Vec2(0, 0);

			if (degenerate)
				target = A;
			else
				target = A + AB * t;

			Vec2 dir = target - Origin;
			dir.Normalize();

			Ray ray = Ray(Origin.X, Origin.Y, dir.X, dir.Y, wg.GenerateWavelength());
			ray.CurrentMedium = this->CurrentMedium;

			rays.push_back(ray);
		}

		return rays;
	}
};

