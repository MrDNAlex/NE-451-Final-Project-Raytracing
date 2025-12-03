#pragma once
#include "Segment.h"
#include <vector>
#include <random>
#include <iostream>
#include <nlohmann/json.hpp>
#include "ObjectNode.h"
#include <limits>
using json = nlohmann::json;

struct FresnelCoeffs
{
	double Reflectance;
	double Transmittance;
};

class Object
{
public:

	class ObjectBounds
	{
	public:
		Vec2 MinBound;

		Vec2 MaxBound;

		ObjectBounds()
		{
			MinBound = Vec2(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
			MaxBound = Vec2(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());
		}

		void GrowToInclude(Segment* segment)
		{
			MinBound.X = std::min(MinBound.X, std::min(segment->A.X, segment->B.X));
			MinBound.Y = std::min(MinBound.Y, std::min(segment->A.Y, segment->B.Y));

			MaxBound.X = std::max(MaxBound.X, std::max(segment->A.X, segment->B.X));
			MaxBound.Y = std::max(MaxBound.Y, std::max(segment->A.Y, segment->B.Y));
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
			{
				double segCenterX = 0.5 * (segment->A.X + segment->B.X);
				return segCenterX <= center;
			}
			else
			{
				double segCenterY = 0.5 * (segment->A.Y + segment->B.Y);
				return segCenterY <= center;
			}
		}


		//Create an Intersect Function
	};

	class ObjectNode
	{
	public:

		ObjectNode* LeftNode;

		ObjectNode* RightNode;

		//Object* Root;

		std::vector<Segment*> Segments;

		ObjectBounds Bounds;

		ObjectNode()
		{
			Segments = std::vector<Segment*>();
			LeftNode = nullptr;
			RightNode = nullptr;
			//Root = root;
			Bounds = ObjectBounds();
		}

		~ObjectNode()
		{
			std::cout << "Deleting ObjectNode" << std::endl;
			//if (LeftNode != nullptr)
			//	delete LeftNode;
			//if (RightNode != nullptr)
			//	delete RightNode;
		}
	};

	std::vector<Segment> Segments;

	std::string Type;

	ObjectNode Root;

	const int MAX_DEPTH = 10;

	Object() : Root()
	{
		Segments = std::vector<Segment>();
		Type = "Object";
	}

	void AddSegment(double x1, double y1, double x2, double y2, std::function<double(double)> refractiveIndex)
	{
		Segments.emplace_back(x1, y1, x2, y2, refractiveIndex);
	}

	void BVH()
	{
		for (Segment& segment : Segments)
		{
			Root.Bounds.GrowToInclude(&segment);
			Root.Segments.push_back(&segment);
		}

		Split(Root, 0);
	}

	void Split(ObjectNode& parent, int depth = 0)
	{
		//Base Case
		if (depth == MAX_DEPTH || parent.Segments.size() <= 1)
			return;

		bool isSplitX = parent.Bounds.LargestDimensionIsX();
		double center = isSplitX ? parent.Bounds.GetCenterX() : parent.Bounds.GetCenterY();

		std::vector<Segment*> leftSegments;
		std::vector<Segment*> rightSegments;

		for (Segment* segment : parent.Segments)
		{
			bool isInLeftNode = parent.Bounds.IsInLeftNode(segment, isSplitX, center);
			if (isInLeftNode)
			{
				leftSegments.push_back(segment);
			}
			else
			{
				rightSegments.push_back(segment);
			}
				
		}

		if (leftSegments.empty() || rightSegments.empty())
		{
			std::cout << "Warning: BVH Split resulted in empty node at depth " << depth << std::endl;
			return;
		}

		parent.LeftNode = new ObjectNode();
		parent.RightNode = new ObjectNode();

		for (Segment* segment : leftSegments)
		{
			parent.LeftNode->Bounds.GrowToInclude(segment);
			parent.LeftNode->Segments.push_back(segment);
		}

		for (Segment* segment : rightSegments)
		{
			parent.RightNode->Bounds.GrowToInclude(segment);
			parent.RightNode->Segments.push_back(segment);
		}

		Split(*parent.LeftNode, depth + 1);
		Split(*parent.RightNode, depth + 1);

		//Needs to be Implemented
	}

	RayHit Intersect(Ray* ray)
	{
		double minT = INFINITY;
		Segment* closestSegment = nullptr;

		for (Segment& segment : Segments)
		{
			RayHit hit = segment.Intersect(ray);

			if (hit.Hit && hit.Distance < minT)
			{
				minT = hit.Distance;
				closestSegment = &segment;
			}
		}

		if (closestSegment == nullptr)
			return RayHit(false, 0.0, nullptr);
		else
			return RayHit(true, minT, closestSegment);
	}

	virtual std::vector<Ray> InteractWithRay(Segment* segment, Ray* ray)
	{
		FresnelCoeffs fresnel = GetFresnelCoefficients(segment, ray);

		Ray cloneRay = ray->Clone();

		std::vector<Ray> resultingRays;

		ray->Power *= fresnel.Reflectance;
		cloneRay.Power *= fresnel.Transmittance;

		segment->Reflect(ray);
		resultingRays.push_back(*ray);

		segment->Transmit(&cloneRay);
		cloneRay.CurrentMedium = segment->GetRefractiveIndex(cloneRay.Wavelength);
		cloneRay.CurrentBounce = 0;
		resultingRays.push_back(cloneRay);

		return resultingRays;
	}

	std::vector<double> linspace(double start, double end, int num) {
		std::vector<double> result;
		if (num <= 0) return result;
		if (num == 1) {
			result.push_back(start);
			return result;
		}

		double step = (end - start) / (num - 1);
		for (int i = 0; i < num; ++i)
			result.push_back(start + step * i);

		return result;
	}

	virtual json ToJSON()
	{
		json j;
		j["Type"] = Type;
		j["SegmentCount"] = Segments.size();
		j["Segments"] = json::array();

		for (Segment& segment : Segments)
			j["Segments"].push_back(segment.ToJSON());

		return j;
	}

protected:
	FresnelCoeffs GetFresnelCoefficients(Segment* segment, Ray* ray)
	{
		double n1 = ray->CurrentMedium;
		double n2 = segment->GetRefractiveIndex(ray->Wavelength);

		Vec2 normal = segment->GetNormal();

		normal.Normalize();
		ray->Direction.Normalize();

		double incidentCos = -normal.Dot(ray->Direction);

		if (incidentCos < 0)
		{
			normal = normal * -1;
			incidentCos = -incidentCos;
		}

		double incidentSin = std::sqrt(std::max(0.0, 1.0 - incidentCos * incidentCos));

		double transmitSin = (n1 / n2) * incidentSin;

		if (transmitSin >= 1.0)
			return FresnelCoeffs{ 1.0, 0.0 };

		double transmitCos = std::sqrt(std::max(0.0, 1.0 - transmitSin * transmitSin));

		double n1ICos = n1 * incidentCos;
		double n2TCos = n2 * transmitCos;

		double n1TCos = n1 * transmitCos;
		double n2ICos = n2 * incidentCos;

		double denomS = n1ICos + n2TCos;
		double denomP = n2ICos + n1TCos;

		if (std::abs(denomS) < EPSILON)
			if (denomS > 0)
				denomS = EPSILON;
			else
				denomS = -EPSILON;

		if (std::abs(denomP) < EPSILON)
			if (denomP > 0)
				denomP = EPSILON;
			else
				denomP = -EPSILON;

		double inverseS = 1.0 / denomS;
		double inverseP = 1.0 / denomP;

		double rs = (n1ICos - n2TCos) * inverseS;
		double ts = (2 * n1ICos) * inverseS;

		double rp = (n2ICos - n1TCos) * inverseP;
		double tp = (2 * n1ICos) * inverseP;

		double Rs = rs * rs;
		double Rp = rp * rp;

		double R = 0.5 * (Rs + Rp);
		double T = 1 - R;

		if (R > 1.0)
			std::cout << "Warning: Reflectance > 1.0 :" << R << std::endl;

		if (T < 0.0)
			std::cout << "Warning: Transmittance < 0.0 : " << T << std::endl;

		return FresnelCoeffs{ R, T };
	}

	int randomInt(int min, int max) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(min, max);
		return dist(gen);
	}
};