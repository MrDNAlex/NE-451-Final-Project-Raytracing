#pragma once
#include <random>
class GaussianDistribution
{
public:
	
	double Mu;

	double Sigma;

	std::mt19937 Generator;
	std::normal_distribution<double> Distribution;

	GaussianDistribution(double mu, double sigma) : Mu(mu), Sigma(sigma), Generator(std::random_device{}()), Distribution(mu, sigma)
	{
	}

	double GetRandomValue()
	{
		return Distribution(Generator);
	}
};

