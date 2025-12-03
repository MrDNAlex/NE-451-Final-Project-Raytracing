#pragma once
#include "Object.h"
class Mirror : public Object
{
public:

	Mirror(double x1, double y1, double x2, double y2) : Object()
	{
		this->AddSegment(x1, y1, x2, y2, [](double) {return 1.0;}, new ConstantPerturbance(0));
		Type = "Mirror";
	}

	std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray) override
	{
		segment->Reflect(ray);

		std::vector<Ray> resultingRays;
		resultingRays.push_back(*ray);

		return resultingRays;
	}
};

