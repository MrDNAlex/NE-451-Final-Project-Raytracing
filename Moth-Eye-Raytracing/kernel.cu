#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include "Object.h"
#include "Ray.h"
#include "RayHit.h"
#include "Scene.h"
#include "Mirror.h"
#include "Target.h"
#include "Wave.h"
#include <functional>
#include "PointSource.h"
#include "QuantumDot.h"
#include "DirectionalLight.h"
#include "ConeLight.h"
#include "DisturbedSegment.h"
#include "FYDPSims.h"
#include "NE451Sims.h"

int main()
{
	// Functions to Run
	//RunMaxCaptureAngleWaveguide();
	//RunQDInternalReflection();
	RunRealLifeTests();
	//RunWaveCalculations();

	//GaussianDistribution gaus = GaussianDistribution(0, 5);
	//
	//Segment seg = Segment(0, 0, 1, 0, [](double wavelength) { return 1.0; });
	//DisturbedSegment distSeg = DisturbedSegment(0.0, 0.0, 1.0, 0.0, [](double wavelength) { return 1.0; }, &gaus);
	//
	//for (int i = 0; i < 100; i++)
	//{
	//	Vec2 normal = seg.GetNormal();
	//	Vec2 distNormal = distSeg.GetNormal();
	//
	//	std::cout << "Normal : X = " << normal.X << ", Y = " << normal.Y << std::endl;
	//	std::cout << "Disturbed Normal : X = " << distNormal.X << ", Y = " << distNormal.Y << std::endl;
	//}

	//Wave wave = Wave(0, 0, 1000, 0, 10000, [](double x, double y) { return MothEyeRefractiveIndex(y); }, 1, 1, 0, 0);
	//
	//wave.BVH();
	//
	//int depth = 0;
	//
	//Object::ObjectNode node = wave.Root;
	//
	//for (int i = 0; i < 10; i++)
	//{
	//	std::cout << "Node " << i << " : MinX = " << node.Bounds.MinBound.X << ", MaxX = " << node.Bounds.MaxBound.X << ", MinY = " << node.Bounds.MinBound.Y << ", MaxY = " << node.Bounds.MaxBound.Y << ", Depth = " << depth << ", Segments = " << node.Segments.size() << std::endl;
	//
	//	if (node.LeftNode != nullptr)
	//	{
	//		node = *(node.LeftNode);
	//	}	
	//	else
	//		break;
	//
	//	depth++;
	//}


	std::cout << "Press ENTER to exit...";
	std::cin.get();
}
