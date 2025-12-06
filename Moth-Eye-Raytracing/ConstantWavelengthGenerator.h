#pragma once
#include "WavelengthGenerator.h"
class ConstantWavelengthGenerator : public WavelengthGenerator
{
public:
	double Wavelength;

	ConstantWavelengthGenerator(double wavelength)
	{
		Wavelength = wavelength;
	}

	double GenerateWavelength() override
	{
		return Wavelength;
	}
};

