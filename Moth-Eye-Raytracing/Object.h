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

		Segment Edges[4];

		ObjectBounds()
			: MinBound(std::numeric_limits<double>::max(),
				std::numeric_limits<double>::max()),
			MaxBound(std::numeric_limits<double>::lowest(),
				std::numeric_limits<double>::lowest()),
			Edges{
				Segment(0,0,0,0, [](double) { return 1.0; }),
				Segment(0,0,0,0, [](double) { return 1.0; }),
				Segment(0,0,0,0, [](double) { return 1.0; }),
				Segment(0,0,0,0, [](double) { return 1.0; })
				}
		{
		}

		void GrowToInclude(Segment* segment)
		{
			MinBound.X = std::min(MinBound.X, std::min(segment->A.X, segment->B.X));
			MinBound.Y = std::min(MinBound.Y, std::min(segment->A.Y, segment->B.Y));

			MaxBound.X = std::max(MaxBound.X, std::max(segment->A.X, segment->B.X));
			MaxBound.Y = std::max(MaxBound.Y, std::max(segment->A.Y, segment->B.Y));

			Edges[0] = Segment(MinBound.X, MinBound.Y, MaxBound.X, MinBound.Y, [](double) {return 1.0; });
			Edges[1] = Segment(MaxBound.X, MinBound.Y, MaxBound.X, MaxBound.Y, [](double) {return 1.0; });
			Edges[2] = Segment(MaxBound.X, MaxBound.Y, MinBound.X, MaxBound.Y, [](double) {return 1.0; });
			Edges[3] = Segment(MinBound.X, MaxBound.Y, MinBound.X, MinBound.Y, [](double) {return 1.0; });
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
	};

	class ObjectNode
	{
	public:

		ObjectNode* LeftNode;

		ObjectNode* RightNode;

		std::vector<Segment*> Segments;

		ObjectBounds Bounds;

		ObjectNode()
		{
			Segments = std::vector<Segment*>();
			LeftNode = nullptr;
			RightNode = nullptr;
			Bounds = ObjectBounds();
		}

		~ObjectNode()
		{
			if (LeftNode != nullptr)
				delete LeftNode;
			if (RightNode != nullptr)
				delete RightNode;
		}
	};

	std::vector<Segment> Segments;

	std::string Type;

	ObjectNode Root;

	const int MAX_DEPTH = 50;

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
		if (depth == MAX_DEPTH || parent.Segments.size() <= depth * 4)
			return;

		bool isSplitX = parent.Bounds.LargestDimensionIsX();
		double center = isSplitX ? parent.Bounds.GetCenterX() : parent.Bounds.GetCenterY();

		std::vector<Segment*> leftSegments;
		std::vector<Segment*> rightSegments;

		for (Segment* segment : parent.Segments)
		{
			bool isInLeftNode = parent.Bounds.IsInLeftNode(segment, isSplitX, center);
			if (isInLeftNode)
				leftSegments.push_back(segment);
			else
				rightSegments.push_back(segment);
		}

		if (leftSegments.empty() || rightSegments.empty())
			return;

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
	}

	//RayHit Intersect(Ray* ray)
	//{
	//	double minT = INFINITY;
	//	Segment* closestSegment = nullptr;
	//
	//	for (Segment& segment : Segments)
	//	{
	//		RayHit hit = segment.Intersect(ray);
	//
	//		if (hit.Hit && hit.Distance < minT)
	//		{
	//			minT = hit.Distance;
	//			closestSegment = &segment;
	//		}
	//	}
	//
	//	if (closestSegment == nullptr)
	//		return RayHit(false, 0.0, nullptr);
	//	else
	//		return RayHit(true, minT, closestSegment);
	//}

	RayHit Intersect(Ray* ray)
	{
		return IntersectNode(&Root, ray);
	}

	RayHit IntersectNode(ObjectNode* node, Ray* ray)
	{
		if (node == nullptr)
			return RayHit(false, 0.0, nullptr);

		if (!node->Bounds.Intersects(ray))
			return RayHit(false, 0.0, nullptr);

		if (node->LeftNode == nullptr && node->RightNode == nullptr)
		{
			double minT = INFINITY;
			Segment* closestSegment = nullptr;

			for (Segment* segment : node->Segments)
			{
				RayHit hit = segment->Intersect(ray);

				if (hit.Hit && hit.Distance < minT)
				{
					minT = hit.Distance;
					closestSegment = segment;
				}
			}

			if (closestSegment == nullptr)
				return RayHit(false, 0.0, nullptr);
			else
				return RayHit(true, minT, closestSegment);
		}

		RayHit leftHit = IntersectNode(node->LeftNode, ray);
		RayHit rightHit = IntersectNode(node->RightNode, ray);

		if (leftHit.Hit && rightHit.Hit)
		{
			if (leftHit.Distance < rightHit.Distance)
				return leftHit;
			else
				return rightHit;
		}
		else if (leftHit.Hit)
			return leftHit;
		else if (rightHit.Hit)
			return rightHit;
		else
			return RayHit(false, 0.0, nullptr);
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