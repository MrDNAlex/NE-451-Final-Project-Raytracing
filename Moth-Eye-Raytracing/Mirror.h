#pragma once
#include "Object.h"
class Mirror : public Object
{
public:

	ConstantPerturbance PerturbanceGenerator;

	Mirror(double x1, double y1, double x2, double y2) : Object(), PerturbanceGenerator(0)
	{
		Type = "Mirror";

		this->AddSegment(x1, y1, x2, y2, [](double) {return 1.0;}, &PerturbanceGenerator);
	}

	std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray) override
	{
		segment->Reflect(ray);

		std::vector<Ray> resultingRays;
		resultingRays.push_back(*ray);

		return resultingRays;
	}
};

