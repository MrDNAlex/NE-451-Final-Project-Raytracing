#pragma once
class Segment;

class RayHit
{
public:

	bool Hit;

	double Distance;

	Segment* SegmentHit;

	RayHit(bool hit, double distance, Segment* segment) : Hit(hit), Distance(distance)
	{
		this->SegmentHit = segment;
	}
};

