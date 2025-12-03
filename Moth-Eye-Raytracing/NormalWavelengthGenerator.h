#pragma once
#include "WavelengthGenerator.h"
#include "GaussianDistribution.h"
class NormalWavelengthGenerator : public WavelengthGenerator
{

public:

	GaussianDistribution Distribution;

	NormalWavelengthGenerator(double mu, double sigma) : Distribution(mu, sigma)
	{
	}

	double GenerateWavelength() override
	{
		return Distribution.GetRandomValue();
	}
};

