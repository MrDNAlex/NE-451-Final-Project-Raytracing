#pragma once
#include "Object.h"
class Target : public Object
{
public:

	double CapturedPower;

	double CapturedRays;

	Target(double x1, double y1, double x2, double y2) : Object()
	{
		this->Type = "Target";
		this->AddSegment(x1, y1, x2, y2, [](double) {return 1.0;});

		this->CapturedPower = 0.0;
		this->CapturedRays = 0.0;
	}

	std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray) override
	{
		this->CapturedPower += ray->Power;
		this->CapturedRays += 1.0;

		return std::vector<Ray>();
	}

	json ToJSON() override
	{
		json j = Object::ToJSON();
		j["CapturedPower"] = this->CapturedPower;
		j["CapturedRays"] = this->CapturedRays;
		return j;
	}
};