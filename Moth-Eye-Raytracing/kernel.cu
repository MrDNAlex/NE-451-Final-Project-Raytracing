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
#include "SolarSource.h"
#include "DisturbedSegment.h"

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

double MothEyeRefractiveIndex(double height)
{
	double A = 1.41225607;
	double B = 0.00122535861;
	double C = 0.0000549792342;
	double D = 0.000000982578598;
	double E = 0.00000000532593744;
	double F = 0.00000000000920959868;

	double h = height;
	double h2 = h * h;
	double h3 = h2 * h;
	double h4 = h3 * h;
	double h5 = h4 * h;

	return A - B * h + C * h2 - D * h3 + E * h4 - F * h5;
}

Scene CreateWaveguideBlock(std::string name, int waveguideLayers, double startX = -10000.0, double endX = 10000.0, bool useMothEyeIndex = false)
{
	Scene scene = Scene(name);

	double mothEyeHeight = 250.0;

	double waveguideTopLeftX = startX;
	double waveguideTopLeftY = 250.0;
	double waveguideTopRightX = endX;
	double waveguideTopRightY = 250.0;
	double waveguideBottomLeftX = startX;
	double waveguideBottomLeftY = 0.0;
	double waveguideBottomRightX = endX;
	double waveguideBottomRightY = 0.0;

	double targetY = -200.0;

	scene.AddObject(new Mirror(waveguideTopLeftX, waveguideTopLeftY, waveguideBottomLeftX, targetY));
	scene.AddObject(new Mirror(waveguideTopRightX, waveguideTopRightY, waveguideBottomRightX, targetY));
	scene.AddObject(new Target(waveguideBottomLeftX, targetY, waveguideBottomRightX, targetY));

	std::vector<double> waveguideRefractiveIndex = linspace(1.0, 1.41, waveguideLayers);
	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double n = waveguideRefractiveIndex[i];

		if (useMothEyeIndex)
			n = MothEyeRefractiveIndex(mothEyeHeight - wy);

		Object* obj = new Object();

		obj->AddSegment(startX, wy, endX, wy, [n](double) {return n;});

		scene.AddObject(obj);
	}

	return scene;
}

//
// Moth Eye Wave Section
//
void QDInternalWaveUnitCell(int QDs, int rays)
{
	std::string name = "QDInternal" + std::to_string(QDs) + "WaveUnitCell";

	Scene scene = Scene(name);

	//Constants
	int waveResolution = 10000;
	int QDResolution = 2000;
	double qdRadius = 5.0;
	double mothEyeHeight = 250.0;
	double pi = 3.14159265358979323846;

	double A = mothEyeHeight / 2.0;
	double B = 2.0 * pi / mothEyeHeight;
	double C = mothEyeHeight / 4.0;
	double D = mothEyeHeight / 2.0;

	double startX = 0.0;
	double endX = 250.0;
	double startY = 0.0;
	double endY = 0.0;
	double depth = -200.0;

	scene.AddObject(new Wave(startX, startY, endX, endY, waveResolution, [](double x, double y) { return MothEyeRefractiveIndex(y); }, A, B, C, D));
	scene.AddObject(new Mirror(startX, startY, startX, depth));
	scene.AddObject(new Mirror(endX, startY, endX, depth));
	scene.AddObject(new Target(startX, depth, endX, depth));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		scene.AddObject(new QuantumDot(qdPositionsX[i], -100, qdRadius, QDResolution));
		scene.AddRaySource(new PointSource(qdPositionsX[i], -100, rays, 1.41));
	}

	std::cout << "Rendering : " << name;

	scene.Render();

	std::cout << "Render Complete" << std::endl;
}

void ConeWaveUnitCell(int QDs, int rays)
{
	std::string name = "Cone" + std::to_string(QDs) + "WaveUnitCell";

	Scene scene = Scene(name);

	//Constants
	int waveResolution = 10000;
	double mothEyeHeight = 250.0;
	double pi = 3.14159265358979323846;

	double A = mothEyeHeight / 2.0;
	double B = 2.0 * pi / mothEyeHeight;
	double C = mothEyeHeight / 4.0;
	double D = mothEyeHeight / 2.0;

	double startX = 0.0;
	double endX = 250.0;
	double startY = 0.0;
	double endY = 0.0;
	double depth = -200.0;

	scene.AddObject(new Wave(startX, startY, endX, endY, waveResolution, [](double x, double y) { return MothEyeRefractiveIndex(y); }, A, B, C, D));
	scene.AddObject(new Mirror(startX, startY, startX, depth));
	scene.AddObject(new Mirror(endX, startY, endX, depth));
	scene.AddObject(new Target(startX, depth, endX, depth));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = startY;

		double bx = endX;
		double by = endY;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, rays, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render(true, false, false);
}

void RunWaveCalculations()
{
	int rays = 5000;

	for (int QDs = 1; QDs <= 20; QDs++)
	{
		QDInternalWaveUnitCell(QDs, rays);
		ConeWaveUnitCell(QDs, rays);
		std::cout << "Completed QDs : " << QDs << std::endl;
	}
}

//
// Moth Eye Waveguide Section
//

void MaxCaptureAngleWaveguide(int waveguideLayers, int numberOfAngles, double endAngle, bool useMothEyeIndex = false)
{
	double startX = -10000.0;
	double endX = 10000.0;
	double emitterLength = 1000;

	double startAngle = 0.0;

	std::vector<double> angles = linspace(startAngle, endAngle, numberOfAngles);

	json js;

	js["Angle"] = json::array();
	js["Power"] = json::array();

	for (int i = 0; i < angles.size(); i++)
	{
		double pi = 3.14159265358979323846;
		double angle = angles[i];
		double radians = angle * pi / 180.0;

		double xStart = -(cos(radians) * emitterLength) + 9900;
		double yStart = sin(radians) * emitterLength + 300;

		std::string name = "MaxCaptureAngleWaveguide" + std::to_string(waveguideLayers) + "Angle" + std::to_string(angle).substr(6) + "MaxAngle" + std::to_string(endAngle);

		Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

		scene.AddObject(new Mirror(startX, 1400.0, startX, 0));
		scene.AddObject(new Mirror(endX, 1400.0, endX, 0));

		scene.AddRaySource(new DirectionalLight(xStart, yStart, 9900, 300, 1));

		scene.Render(false, false, false);

		for (int j = 0; j < scene.Objects.size(); j++)
		{
			if (scene.Objects[j]->Type == "Target")
			{
				Target* target = static_cast<Target*>(scene.Objects[j]);

				js["Power"].push_back(target->CapturedPower / (double)scene.Stats.StartRays);
				js["Angle"].push_back(angle);
			}
		}
	}

	std::string name = "MaxCaptureAngleWaveguide" + std::to_string(waveguideLayers);

	if (useMothEyeIndex)
		name += "_MothEye";

	std::ofstream file(name + ".json");
	file << js.dump(2);
	file.close();
}

void RunMaxCaptureAngleWaveguide()
{
	std::vector<double> waveGuides = { 2, 3, 4, 5 , 6, 7, 8, 9, 10 };
	std::vector<double> waveGuides1 = linspace(10, 250, 24);

	waveGuides.insert(waveGuides.end(), waveGuides1.begin(), waveGuides1.end());

	double maxAngle1 = 90.0;
	double maxAngle2 = 60.0;
	int numberOfAngles = 10000;

	for (int i = 0; i < waveGuides.size(); i++)
	{
		int layers = (int)waveGuides[i];
		MaxCaptureAngleWaveguide(layers, numberOfAngles, maxAngle1, false);
		MaxCaptureAngleWaveguide(layers, numberOfAngles, maxAngle1, true);
		MaxCaptureAngleWaveguide(layers, numberOfAngles, maxAngle2, false);
		MaxCaptureAngleWaveguide(layers, numberOfAngles, maxAngle2, true);

		std::cout << "Completed Max Capture Layers : " << layers << " Completion : " << 100.0 * (double)i / (double)waveGuides.size() << "%" << std::endl;
	}
}

void ConeWaveguide(int QDs, int waveguideLayers, int raysPerCone = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -10000.0;
	double endX = 10000.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;

	std::string name = "Cone" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Large";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, raysPerCone, 1.41));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void ConeWaveguideUnitCell(int QDs, int waveguideLayers, int raysPerCone = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -125.0;
	double endX = 125.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;

	std::string name = "Cone" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "UnitCell";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, raysPerCone, 1.41));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void QDWaveguideUnitCell(int QDs, int waveguideLayers, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -125.0;
	double endX = 125.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	int QDResolution = 500;

	std::string name = "QD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "UnitCell";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
		scene.AddRaySource(new PointSource(ox, oy, raysPerQD, 1.41));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void QDWaveguide(int QDs, int waveguideLayers, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -10000.0;
	double endX = 10000.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	int QDResolution = 500;

	std::string name = "QD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Large";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
		scene.AddRaySource(new PointSource(ox, oy, raysPerQD, 1.41));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void RunQDInternalReflection()
{
	int rays = 250;

	std::vector<double> waveGuides = { 2, 4, 6, 8, 10 };
	std::vector<double> waveGuides1 = linspace(10, 250, 12);

	waveGuides.insert(waveGuides.end(), waveGuides1.begin(), waveGuides1.end());

	for (int QDs = 1; QDs <= 20; QDs++)
	{
		for (int i = 0; i < waveGuides.size(); i++)
		{
			int layers = (int)waveGuides[i];
			QDWaveguide(QDs, layers, rays, false);
			QDWaveguide(QDs, layers, rays, true);
			QDWaveguideUnitCell(QDs, layers, rays, false);
			QDWaveguideUnitCell(QDs, layers, rays, true);
			ConeWaveguide(QDs, layers, rays, false);
			ConeWaveguide(QDs, layers, rays, true);
			ConeWaveguideUnitCell(QDs, layers, rays, false);
			ConeWaveguideUnitCell(QDs, layers, rays, true);

			std::cout << "Completed QDs : " << QDs << " Layers : " << layers << " Completion : " << 100.0 * (double)(i + (QDs - 1) * waveGuides.size()) / (double)(waveGuides.size() * 20) << "%" << std::endl;
		}
	}
}

void RealLifeTestUnitCell(int QDs, int waveguideLayers, double angle, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -125.0;
	double endX = 125.0;
	double qdRadius = 5.0;
	double sourceHeight = 300.0;
	int QDResolution = 250;

	std::string name = "RealLifeQD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Angle" + std::to_string(angle) + "UnitCell";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	scene.AddObject(new Mirror(startX, 1000.0, startX, 0));
	scene.AddObject(new Mirror(endX, 1000.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, raysPerQD, true));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void RealLifeTest(int QDs, int waveguideLayers, double angle, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -10000.0;
	double endX = 10000.0;
	double qdRadius = 5.0;
	double sourceHeight = 300.0;
	int QDResolution = 250;

	std::string name = "RealLifeQD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Angle" + std::to_string(angle);

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	scene.AddObject(new Mirror(startX, 20000.0, startX, 0));
	scene.AddObject(new Mirror(endX, 20000.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, raysPerQD, true));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
	}

	std::cout << "Rendering : " << name << "... ";

	scene.Render(true, false, false);

	std::cout << "Render Complete" << std::endl;
}

void RunRealLifeTests()
{
	int rays = 250;

	std::vector<double> angles = linspace(0.0, 80.0, 4);
	std::vector<double> waveGuides = { 2, 4, 6, 8, 10 };
	std::vector<double> waveGuides1 = linspace(10, 250, 10);

	waveGuides.insert(waveGuides.end(), waveGuides1.begin(), waveGuides1.end());

	for (int j = 0; j < angles.size(); j++)
	{
		double angle = angles[j];

		for (int QDs = 0; QDs <= 500; QDs += 50)
		{
			for (int i = 0; i < waveGuides.size(); i++)
			{
				int layers = (int)waveGuides[i];
				RealLifeTest(QDs, layers, angle, rays);
				RealLifeTestUnitCell(QDs, layers, angle, rays);

				double percent = 100.0 * (double)(i + waveGuides.size() * ((QDs / 50) + j * 11)) / (double)(waveGuides.size() * 11 * angles.size());


				std::cout << "Completed QDs : " << QDs << " Layers : " << layers << " Completion : " << percent << "%" << std::endl;
			}
		}
	}
}

//This is in 
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
