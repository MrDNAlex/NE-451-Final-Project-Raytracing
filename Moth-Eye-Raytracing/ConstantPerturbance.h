#pragma once
#include "PerturbanceGenerator.h"
class ConstantPerturbance : public PerturbanceGenerator
{
public: 

	double Perturbance;

	ConstantPerturbance(double perturbance)
	{
		Perturbance = perturbance;
	}

	double GeneratePerturbance() override
	{
		return Perturbance;
	}
};