#pragma once
#include "PerturbanceGenerator.h"
#include "GaussianDistribution.h"
class NormalPerturbance : public PerturbanceGenerator
{
public:

	GaussianDistribution Distribution;

	NormalPerturbance(double mu, double sigma) : Distribution(mu, sigma)
	{
	}

	double GeneratePerturbance() override
	{
		return Distribution.GetRandomValue();
	}
};