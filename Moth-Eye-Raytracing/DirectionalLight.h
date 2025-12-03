#pragma once
#include "RaySource.h"
#include "Ray.h"
class DirectionalLight : public RaySource
{
public:

	Vec2 A;

	Vec2 B;

	bool Down;

	DirectionalLight(double x1, double y1, double x2, double y2, int numberOfRays, WavelengthGenerator* wavelengthGenerator, bool down = true, double currentMedium = 1.0) : RaySource(numberOfRays, wavelengthGenerator, currentMedium), A(x1, y1), B(x2, y2)
	{
		this->Down = down;	
	}

	std::vector<Ray> GenerateRays() override
	{
		std::vector<Ray> rays;
		std::vector<double> xs = linspace(A.X, B.X, NumberOfRays);
		std::vector<double> ys = linspace(A.Y, B.Y, NumberOfRays);

		Vec2 direction = (B - A).GetNormal();

		direction.Normalize();

		if (Down && direction.Dot(Vec2(0, -1)) < 0)
			direction = direction * -1;
		
		WavelengthGenerator& wg = *this->WavelengthGen;

		for (int i = 0; i < NumberOfRays; i++)
		{
			Vec2 pointOnLine = Vec2(xs[i], ys[i]);
			
			Ray ray = Ray(pointOnLine.X, pointOnLine.Y, direction.X, direction.Y, wg.GenerateWavelength());
			ray.CurrentMedium = this->CurrentMedium;

			rays.push_back(ray);
		}

		return rays;
	}
};