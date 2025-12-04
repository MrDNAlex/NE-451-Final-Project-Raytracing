#pragma once
#include "Object.h"
#include "Vec2.h"
#include "ConstantPerturbance.h"
class QuantumDot : public Object
{
public:

	ConstantPerturbance PerturbanceGen;

	Vec2 Center;

	double Radius;

	QuantumDot(double x, double y, double radius, int resolution) : Object(), Center(x, y), Radius(radius), PerturbanceGen(0)
	{
		Type = "QuantumDot";

		std::vector<double> theta = linspace(0.0, 2 * 3.14159265358979323846, resolution);

		for (int i = 0; i < resolution; i++)
		{
			double x1 = this->Center.X + radius * cos(theta[i]);
			double y1 = this->Center.Y + radius * sin(theta[i]);

			double x2 = this->Center.X + radius * cos(theta[(i + 1) % resolution]);
			double y2 = this->Center.Y + radius * sin(theta[(i + 1) % resolution]);

			this->AddSegment(x1, y1, x2, y2, [](double) {return 1.0; }, &PerturbanceGen);
		}
	}

	std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray) override
	{
		int randInt = randomInt(0, this->Segments.size() - 1);

		Segment randomSegment = this->Segments[randInt];

		Vec2 normal = randomSegment.GetNormal();
		Vec2 newOrigin = this->Center + normal * this->Radius * 1.1;

		ray->Origin = newOrigin;
		ray->Direction = normal;
		//ray->CurrentBounce = 0;

		std::vector<Ray> rays = std::vector<Ray>();
		rays.push_back(*ray);

		return rays;
	}
};