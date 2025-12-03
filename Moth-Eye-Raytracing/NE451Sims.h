#pragma once
#include <functional>
#include <cmath>

double SellmeierMicron(double wavelength)
{
	double B1 = 1.0093;
	double C1 = 0.013185;

	return sqrt(1 + (B1 * wavelength * wavelength) / (wavelength * wavelength - C1));
}

double SellmeierPDMS(double wavelength_nm)
{
	double wavelengthSqr = wavelength_nm * wavelength_nm;
	double B1 = 1.0093;
	double C1 = 13185;

	double nPDMS = sqrt(1 + (B1 * wavelengthSqr) / (wavelengthSqr - C1));

	return nPDMS;
}

double SellmeierAir(double wavelength_nm)
{
	double wavelength_um = wavelength_nm / 1000.0;

	double B1 = 0.05792105;
	double B2 = 0.00167917;
	double C1 = 238.0185;
	double C2 = 57.362;

	double inverseLambda = (1 / (wavelength_um * wavelength_um));

	return 1 + B1 / (C1 - inverseLambda) + B2 / (C2 - inverseLambda);
}

double FillFactor(double heightFraction)
{
	//Constants
	double sqrt3 = sqrt(3);
	double pi = 3.14159265358979323846;
	double radius = 1;

	//Hexagon Values
	double hexagonSideLength = radius * (2 / sqrt3);
	double hexagonArea = 1.5 * sqrt3 * hexagonSideLength * hexagonSideLength;

	//Circle Values
	double circleArea = pi * sqrt(2 * radius * heightFraction - heightFraction * heightFraction);

	return circleArea / hexagonArea;
}

std::function<double(double)> CreateEffectiveRefractiveIndexFunction(double heightFraction)
{
	double fillFactor = FillFactor(heightFraction);
	double q = 2.0 / 3.0;

	return [fillFactor, q](double wavelength)
		{
			return pow(fillFactor * pow(SellmeierPDMS(wavelength), q) + (1 - fillFactor) * pow(SellmeierAir(wavelength * 0.001), q), 1 / q);
		};
}

double GetRefractiveIndexPDMS(double x, double y, double wavelength)
{
	double heightFrac = (250.0 - y) / 250.0;

	double nAir = 1.0;
	double nPDMS = SellmeierPDMS(wavelength);

	return nAir + (nPDMS - nAir) * heightFrac;
}
