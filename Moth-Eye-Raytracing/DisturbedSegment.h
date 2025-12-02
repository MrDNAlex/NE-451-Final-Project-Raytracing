#pragma once
#include "Segment.h"
#include <random>
#include "GaussianDistribution.h"
class DisturbedSegment : public Segment
{
public:

	GaussianDistribution* NormalDistribution; 

	DisturbedSegment(double x1, double y1, double x2, double y2, double refractiveIndex, GaussianDistribution* distribution, std::function<double(double)> refractiveIndexFunc = nullptr) : Segment(x1, y1, x2, y2, refractiveIndex, refractiveIndexFunc), NormalDistribution(distribution)
	{
	}

	Vec2 GetNormal(bool left = true)
	{
		Vec2 dir = B - A;
		Vec2 normal = Vec2(0, 0);

		if (left)
			normal = Vec2(-dir.Y, dir.X);
		else
			normal = Vec2(dir.Y, -dir.X);

		double disturbance = NormalDistribution->GetRandomValue();

		normal = normal.Rotate(disturbance);
		normal.Normalize();

		return normal;
	}
};