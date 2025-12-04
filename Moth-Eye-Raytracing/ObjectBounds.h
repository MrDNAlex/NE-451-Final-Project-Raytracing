#pragma once
#include "Segment.h"
#include "ConstantPerturbance.h"
class ObjectBounds
{
public:
	Vec2 MinBound;

	Vec2 MaxBound;

	Segment Edges[4];

	Vec2 Corners[4];

	ObjectBounds()
		: MinBound(std::numeric_limits<double>::max(),
			std::numeric_limits<double>::max()),
		MaxBound(std::numeric_limits<double>::lowest(),
			std::numeric_limits<double>::lowest()),
		Edges{
			Segment(0,0,0,0, [](double) { return 1.0; }, new ConstantPerturbance(0)),
			Segment(0,0,0,0, [](double) { return 1.0; }, new ConstantPerturbance(0)),
			Segment(0,0,0,0, [](double) { return 1.0; }, new ConstantPerturbance(0)),
			Segment(0,0,0,0, [](double) { return 1.0; }, new ConstantPerturbance(0))
			}
	{
	}

	void GrowToInclude(Segment* segment)
	{
		MinBound.X = std::min(MinBound.X, std::min(segment->A.X, segment->B.X));
		MinBound.Y = std::min(MinBound.Y, std::min(segment->A.Y, segment->B.Y));

		MaxBound.X = std::max(MaxBound.X, std::max(segment->A.X, segment->B.X));
		MaxBound.Y = std::max(MaxBound.Y, std::max(segment->A.Y, segment->B.Y));

		Edges[0] = Segment(MinBound.X, MinBound.Y, MaxBound.X, MinBound.Y, [](double) {return 1.0; }, new ConstantPerturbance(0));
		Edges[1] = Segment(MaxBound.X, MinBound.Y, MaxBound.X, MaxBound.Y, [](double) {return 1.0; }, new ConstantPerturbance(0));
		Edges[2] = Segment(MaxBound.X, MaxBound.Y, MinBound.X, MaxBound.Y, [](double) {return 1.0; }, new ConstantPerturbance(0));
		Edges[3] = Segment(MinBound.X, MaxBound.Y, MinBound.X, MinBound.Y, [](double) {return 1.0; }, new ConstantPerturbance(0));

		Corners[0] = Vec2(MinBound.X, MinBound.Y);
		Corners[1] = Vec2(MaxBound.X, MinBound.Y);
		Corners[2] = Vec2(MaxBound.X, MaxBound.Y);
		Corners[3] = Vec2(MinBound.X, MaxBound.Y);
	}

	bool LargestDimensionIsX()
	{
		double xLength = MaxBound.X - MinBound.X;
		double yLength = MaxBound.Y - MinBound.Y;

		return xLength >= yLength;
	}

	double GetCenterX()
	{
		return 0.5 * (MinBound.X + MaxBound.X);
	}

	double GetCenterY()
	{
		return 0.5 * (MinBound.Y + MaxBound.Y);
	}

	bool IsInLeftNode(Segment* segment, bool xLargestDim, double center)
	{
		if (xLargestDim)
			return segment->GetCenterX() <= center;
		else
			return segment->GetCenterY() <= center;
	}

	bool Intersects(Ray* ray)
	{
		RayHit hit1 = Edges[0].Intersect(ray);
		if (hit1.Hit)
			return true;

		RayHit hit2 = Edges[1].Intersect(ray);
		if (hit2.Hit)
			return true;

		RayHit hit3 = Edges[2].Intersect(ray);
		if (hit3.Hit)
			return true;

		RayHit hit4 = Edges[3].Intersect(ray);
		if (hit4.Hit)
			return true;

		return false;
	}

	Vec2 GetCorner(int index)
	{
		// 0: Bottom-Left
		// 1: Bottom-Right
		// 2: Top-Right
		// 3: Top-Left

		return Corners[index];
	}
};

