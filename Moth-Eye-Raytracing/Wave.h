#pragma once
#include "Vec2.h"
#include "Object.h"
#include <functional>
class Wave : public Object
{
public:

	Wave(double x1, double y1, double x2, double y2, int resolution, std::function<double(double, double)> refractiveIndex, PerturbanceGenerator* generator, double A = 1.0, double B = 1.0, double C = 1.0, double D = 1.0) : Object()
	{
		Type = "Wave";
		std::vector<double> x = linspace(x1, x2, resolution);
		std::vector<double> yShift = linspace(y1, y2, resolution);

		for (int i = 0; i < resolution; i++)
		{
			double phase = B * (x[i] - C);
			yShift[i] += A * sin(phase) + D;
		}

		for (int i = 0; i < resolution - 1; i++)
		{
			double n = refractiveIndex(x[i], yShift[i]);
			this->AddSegment(x[i], yShift[i], x[i + 1], yShift[i + 1], [n](double wavelength) {return n; }, generator);
		}
	}
};