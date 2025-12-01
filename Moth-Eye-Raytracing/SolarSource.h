#pragma once
#include "RaySource.h"
#include <iostream>
#include <fstream>
#include <random>
class SolarSource : public RaySource
{
public:

	Vec2 Origin;

	Vec2 Direction;
	
	SolarSource(double ox, double oy, double dx, double dy, int numberOfRays, double currentMedium = 1.0) : RaySource(numberOfRays, currentMedium), Origin(ox, oy), Direction(dx, dy)
	{
	}

	std::vector<double> GenerateWavelengths()
	{
		//Load the solar spectrum data from a file or define it here

		std::ifstream file("AM15_spectrum.json");
		if (!file.is_open()) {
			std::cerr << "Error: could not open JSON file\n";
			return std::vector<double>();
		}

		json AMDist;

		file >> AMDist;

		std::vector<double> sampledWavelengths = std::vector<double>();
		std::vector<double> wavelengths = AMDist["Wavelength"].get<std::vector<double>>();
		std::vector<double> probability = AMDist["Probability"].get<std::vector<double>>();

		std::discrete_distribution<int> dist(probability.begin(), probability.end());

		std::random_device rd;
		
		for (int i = 0; i < NumberOfRays; ++i)
		{
			std::mt19937 gen(rd());
			int index = dist(gen);
			sampledWavelengths.push_back(wavelengths[index]);
		}

		return sampledWavelengths;
	}

	std::vector<double> GenerateAngles()
	{
		std::ifstream file("10DegGaussian.json");
		if (!file.is_open()) {
			std::cerr << "Error: could not open JSON file\n";
			return std::vector<double>();
		}

		json AMDist;

		file >> AMDist;

		std::vector<double> sampledAngles = std::vector<double>();
		std::vector<double> angles = AMDist["Angle"].get<std::vector<double>>();
		std::vector<double> probability = AMDist["Probability"].get<std::vector<double>>();

		std::discrete_distribution<int> dist(probability.begin(), probability.end());

		std::random_device rd;

		for (int i = 0; i < NumberOfRays; ++i)
		{
			std::mt19937 gen(rd());
			int index = dist(gen);
			sampledAngles.push_back(angles[index]);
		}

		return sampledAngles;
	}

	std::vector<Ray> GenerateRays() override
	{
		std::vector<Ray> rays = std::vector<Ray>();

		std::vector<double> wavelengths = GenerateWavelengths();
		std::vector<double> angles = GenerateAngles();

		for (int i = 0; i < NumberOfRays; ++i)
		{
			Vec2 direction = this->Direction.Rotate(angles[i]);
			Ray ray = Ray(this->Origin.X, this->Origin.Y, direction.X, direction.Y, wavelengths[i]);

			rays.push_back(ray);
		}

		return rays;
	}
};