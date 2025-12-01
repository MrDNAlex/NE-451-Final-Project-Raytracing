#pragma once
#include "RaySource.h"
#include <iostream>
#include <fstream>
#include <random>
class SolarSource : public RaySource
{
public:



	SolarSource(int numberOfRays, double currentMedium = 1.0) : RaySource(numberOfRays, currentMedium)
	{
	}

	std::vector<double> GenerateWavelengths()
	{
		//Load the solar spectrum data from a file or define it here

		std::ifstream file("AM15_spectrum.json");
		if (!file.is_open()) {
			std::cerr << "Error: could not open JSON file\n";
			//return 1;
		}

		json AMDist;

		file >> AMDist;

		std::vector<double> wavelengths = AMDist["Wavelength"].get<std::vector<double>>();
		std::vector<double> probability = AMDist["Probability"].get<std::vector<double>>();

		std::discrete_distribution<int> dist(probability.begin(), probability.end());

		std::vector<double> sampledWavelengths = std::vector<double>();

		std::random_device rd;
		
		for (int i = 0; i < NumberOfRays; ++i)
		{
			std::mt19937 gen(rd());
			int index = dist(gen);
			sampledWavelengths.push_back(wavelengths[index]);
		}

		return sampledWavelengths;
	}

	std::vector<Ray> GenerateRays() override
	{
		std::vector<Ray> rays = std::vector<Ray>();






		//Vec2 AB = B - A;
		//bool degenerate = (std::abs(AB.X) < 1e-12 && std::abs(AB.Y) < 1e-12);
		//
		//for (int i = 0; i < NumberOfRays; ++i)
		//{
		//	double t = (double)i / (double)(NumberOfRays - 1);
		//
		//	Vec2 target = Vec2(0, 0);
		//
		//	if (degenerate)
		//		target = A;
		//	else
		//		target = A + AB * t;
		//
		//	Vec2 dir = target - Origin;
		//	dir.Normalize();
		//
		//	Ray ray = Ray(Origin.X, Origin.Y, dir.X, dir.Y);
		//	ray.CurrentMedium = this->CurrentMedium;
		//
		//	rays.push_back(ray);
		//}


		return rays;
	}

};