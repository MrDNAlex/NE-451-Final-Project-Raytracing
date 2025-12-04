#pragma once
#include "Vec2.h"
#include "Ray.h"
#include "WavelengthGenerator.h"
class RaySource
{
public:

	int NumberOfRays;

	double CurrentMedium;

	WavelengthGenerator* WavelengthGen;

	RaySource(int numberOfRays, WavelengthGenerator* wavelengthGenerator, double currentMedium = 1.0)
	{
		this->NumberOfRays = numberOfRays;
		this->CurrentMedium = currentMedium;
		this->WavelengthGen = wavelengthGenerator;
	}

	~RaySource()
	{
		if (WavelengthGen != nullptr)
		{
			delete WavelengthGen;
			WavelengthGen = nullptr;
		}
	}

	virtual std::vector<Ray> GenerateRays()
	{
		return std::vector<Ray>();
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

	virtual Object* GetObject()
	{
		return new Object();
	}
};