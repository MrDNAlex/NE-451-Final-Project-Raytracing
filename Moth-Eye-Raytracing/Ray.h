#pragma once
#include "Vec2.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class Ray
{
public:

	Vec2 Origin;

	Vec2 Direction;

	int CurrentBounce;

	int MaxBounce;

	int Index;

	double CurrentMedium;

	double Power;

	double OriginalPower;

	double Wavelength;

	Ray(double ox, double oy, double dx, double dy, double wavelength = 500, int currentBounce = 0, double power = 1.0, int maxBounce = 5000, double currentMedium=1.0) : Origin(ox, oy), Direction(dx, dy)
	{
		this->Direction.Normalize();
		this->CurrentMedium = currentMedium;
		this->CurrentBounce = currentBounce;
		this->MaxBounce = maxBounce;
		this->Power = power;
		this->OriginalPower = power;
		this->Wavelength = wavelength;
		this->Index = 0;
	}

	void Bounce()
	{
		this->CurrentBounce++;
	}

	bool DestroyRay()
	{
		return this->CurrentBounce > this->MaxBounce || this->Power < 0.005;
	}

	Ray Clone()
	{
		Ray newRay(this->Origin.X, this->Origin.Y, this->Direction.X, this->Direction.Y, this->Wavelength, this->CurrentBounce, this->Power, this->MaxBounce);
		newRay.CurrentMedium = this->CurrentMedium;
		newRay.Index = this->Index;
		return newRay;
	}

	Vec2 GetIntersectionPosition(double distance)
	{
		return Origin + Direction * distance;
	}

	json ToJSON()
	{
		json j;

		j["Origin"] = Origin.ToJSON();
		j["Direction"] = Direction.ToJSON();
		j["CurrentBounce"] = CurrentBounce;
		j["MaxBounce"] = MaxBounce;
		j["Power"] = Power;
		j["Wavelength"] = Wavelength;
		j["CurrentMedium"] = CurrentMedium;
		j["Index"] = Index;

		return j;
	}
};