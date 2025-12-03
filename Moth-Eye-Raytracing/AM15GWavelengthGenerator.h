#pragma once
#include "WavelengthGenerator.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <random>
using json = nlohmann::json;
class AM15GWavelengthGenerator : public WavelengthGenerator
{
public:

	std::vector<double> Wavelengths;

	std::discrete_distribution<int> Distribution;

	std::mt19937 Generator;

	AM15GWavelengthGenerator() : Generator(std::random_device{}())
	{
		LoadDistribution();
	}

	void LoadDistribution()
	{
		std::cout << "Loading AM1.5G Spectrum from JSON file...\n";

		std::ifstream file("AM15_spectrum.json");
		if (!file.is_open()) {
			std::cerr << "Error: could not open JSON file\n";
			return;
		}

		json AMDist;

		file >> AMDist;

		std::cout << "Loaded JSON";

		std::vector<double> probability = AMDist["Probability"].get<std::vector<double>>();

		std::cout << "Got Distribution\n";

		Wavelengths = AMDist["Wavelength"].get<std::vector<double>>();
		Distribution = std::discrete_distribution<int>(probability.begin(), probability.end());

		std::cout << "AM1.5G Spectrum loaded successfully.\n";
	}

	double GenerateWavelength() override
	{
		int index = Distribution(Generator);
		return Wavelengths[index];
	}
};