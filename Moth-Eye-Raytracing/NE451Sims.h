#pragma once
#include <functional>
#include <cmath>
#include "NormalPerturbance.h"

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

Object* CreateWave(double x1, double y1, double x2, double y2, int resolution, std::function<double(double)> refractiveIndex, PerturbanceGenerator* generator, double A = 1.0, double B = 1.0, double C = 1.0, double D = 1.0)
{
	Object* obj = new Object();


	obj->Type = "Wave";
	std::vector<double> x = linspace(x1, x2, resolution);
	std::vector<double> yShift = linspace(y1, y2, resolution);

	for (int i = 0; i < resolution; i++)
	{
		double phase = B * (x[i] - C);
		yShift[i] += A * sin(phase) + D;
	}

	for (int i = 0; i < resolution - 1; i++)
	{
		obj->AddSegment(x[i], yShift[i], x[i + 1], yShift[i + 1], refractiveIndex, generator);
	}

	return obj;
}

Scene CreateUnitCellWaveguideBlock(std::string name, int waveguideLayers, PerturbanceGenerator* pertubance, double startX = -125, double endX = 125)
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

	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double heightFraction = (mothEyeHeight - wy) / mothEyeHeight;

		Object* obj = new Object();

		obj->AddSegment(startX, wy, endX, wy, CreateEffectiveRefractiveIndexFunction(heightFraction), pertubance);

		scene.AddObject(obj);
	}

	return scene;
}

Scene CreateUnitCellWaveWaveguideBlock(std::string name, int waveguideLayers, PerturbanceGenerator* pertubance, double startX = -125, double endX = 125)
{
	Scene scene = Scene(name);

	int waveResolution = 500;

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

	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	GaussianDistribution gauss = GaussianDistribution(1.2*(endX - startX) , 0.5*(endX - startX));

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double heightFraction = (mothEyeHeight - wy) / mothEyeHeight;

		Object* wave = CreateWave(startX, wy, endX, wy, waveResolution, CreateEffectiveRefractiveIndexFunction(heightFraction), pertubance, 1.0, 2.0 * 3.14159265358979323846 / gauss.GetRandomValue(), 0.0, 0.0);

		scene.AddObject(wave);
	}

	return scene;
}

std::string RunWavelengthSweep(std::string path, int numOfLayers, double wavelength, int numOfRays, int avgIndex, double angle)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string filePath = path + "/AVG_" + std::to_string(avgIndex);
	CreateFolder(filePath);

	std::string name = "WavelengthSweep_Wavelength_" + std::to_string(wavelength);

	Scene scene = CreateUnitCellWaveguideBlock(name, numOfLayers, new ConstantPerturbance(0), startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));


	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new ConstantWavelengthGenerator(wavelength), new ConstantPerturbance(0)));

	scene.Render(true, false, false, true, false, filePath);

	filePath += name;

	return filePath;
}

std::string RunAMG15GLayerSweeps(std::string path, int numOfLayers, int numOfRays, int avgIndex, double angle)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string filePath = path + "/Layers_" + std::to_string(numOfLayers);
	CreateFolder(filePath);

	std::string name = "AM15G_AVG_" + std::to_string(avgIndex);

	Scene scene = CreateUnitCellWaveguideBlock(name, numOfLayers, new ConstantPerturbance(0), startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new AM15GWavelengthGenerator(), new ConstantPerturbance(0)));

	scene.Render(true, false, false, true, true, filePath);

	filePath += name;

	return filePath;
}

std::string RunNormalPerturbance(std::string path, int numOfLayers, int numOfRays, int avgIndex, double angle, double perturbanceDeviation)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string filePath = path + "/Layers_" + std::to_string(numOfLayers);
	CreateFolder(filePath);

	std::string name = "Perturb_AVG_" + std::to_string(avgIndex);

	PerturbanceGenerator* pertubance = nullptr;

	if (perturbanceDeviation > 0)
		pertubance = new NormalPerturbance(0, perturbanceDeviation);
	else
		pertubance = new ConstantPerturbance(0);

	Scene scene = CreateUnitCellWaveguideBlock(name, numOfLayers, pertubance, startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new AM15GWavelengthGenerator(), new ConstantPerturbance(0)));

	scene.Render(true, false, false, true, true, filePath);

	filePath += name;

	return filePath;
}

std::string RunWavyNormalPerturbance(std::string path, int numOfLayers, int numOfRays, int avgIndex, double angle, double perturbanceDeviation)
{
	double startX = -500.0;
	double endX = 500.0;
	double sourceHeight = 300.0;

	std::string filePath = path + "/Layers_" + std::to_string(numOfLayers);
	CreateFolder(filePath);

	std::string name = "Perturb_AVG_" + std::to_string(avgIndex);

	PerturbanceGenerator* pertubance = nullptr;

	if (perturbanceDeviation > 0)
		pertubance = new NormalPerturbance(0, perturbanceDeviation);
	else
		pertubance = new ConstantPerturbance(0);

	Scene scene = CreateUnitCellWaveWaveguideBlock(name, numOfLayers, pertubance, startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new AM15GWavelengthGenerator(), new ConstantPerturbance(0)));

	scene.Render(true, false, false, true, true, filePath);

	filePath += name;

	return filePath;
}

void RunSimulationCategory1()
{
	CreateFolder("Simulations");
	CreateFolder("Simulations/Simulation1_WavelengthSweep");

	std::string filePath = "Simulations/Simulation1_WavelengthSweep/";

	AM15GWavelengthGenerator wg = AM15GWavelengthGenerator();

	int avg = 1; //5
	int numOfRays = 100;
	int maxLayers = 100; //100
	int maxAngle = 60; //60

	int angleStep = 20;
	int layerStep = 20; //5
	int wavelengthStep = 10;

	int totalRuns = ((maxAngle / angleStep) + 1) * ((maxLayers - 5) / layerStep + 1) * avg;
	int simIndex = 0;

	json j;

	for (int a = 0; a <= maxAngle; a += angleStep)
	{
		json j_angle;

		std::cout << "Starting Angle: " << a << " degrees" << std::endl;

		std::string angleName = "Angle_" + std::to_string(a);

		std::string angleFilePath = filePath + angleName;

		CreateFolder(angleFilePath);

		for (int i = 5; i <= maxLayers; i += layerStep)
		{
			json j_layer;

			std::string layerName = "Layers_" + std::to_string(i);

			std::string layerFilePath = angleFilePath + "/" + layerName;

			CreateFolder(layerFilePath);

			for (int k = 0; k < avg; k++)
			{
				auto start = std::chrono::high_resolution_clock::now();

				std::string avgName = "Avg_" + std::to_string(k);

				std::vector<std::string> filePaths = std::vector<std::string>();

				for (int j = 0; j <= wg.Wavelengths.size(); j += wavelengthStep)
				{
					filePaths.push_back(RunWavelengthSweep(layerFilePath, i, wg.Wavelengths[j], numOfRays, k, a));

					double percentComplete = ((double)j / (double)wg.Wavelengths.size()) * 100.0;
				}

				j_layer[avgName] = filePaths;

				simIndex++;

				double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

				auto end = std::chrono::high_resolution_clock::now();

				double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

				std::cout << "Completed Sweep : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;
			}

			j_angle[layerName] = j_layer;

			std::cout << "Completed Layer Count: " << i << std::endl;
		}

		j[angleName] = j_angle;
	}

	std::ofstream file("Simulations/Simulation1_WavelengthSweep/FilePaths.json");
	file << j.dump(2);
	file.close();
}

void RunSimulationCategory2()
{
	CreateFolder("Simulations");
	CreateFolder("Simulations/Simulation2_AM15GSpectrum");

	std::string filePath = "Simulations/Simulation2_AM15GSpectrum/";

	int avg = 1; //5
	int numOfRays = 10000;
	int maxLayers = 100; //100
	int maxAngle = 60; //60

	int angleStep = 20;
	int layerStep = 20;//5

	int totalRuns = ((maxAngle / angleStep) + 1) * ((maxLayers - 5) / layerStep + 1) * avg;
	int simIndex = 0;

	json j;

	for (int a = 0; a <= maxAngle; a += angleStep)
	{
		json j_angle;

		std::cout << "Starting Angle: " << a << " degrees" << std::endl;

		std::string angleName = "Angle_" + std::to_string(a);

		std::string angleFilePath = filePath + angleName;

		CreateFolder(angleFilePath);

		for (int i = 5; i <= maxLayers; i += layerStep)
		{
			json j_layer;

			std::string layerName = "Layers_" + std::to_string(i);

			std::vector<std::string> filePaths = std::vector<std::string>();

			for (int k = 0; k < avg; k++)
			{
				auto start = std::chrono::high_resolution_clock::now();

				filePaths.push_back(RunAMG15GLayerSweeps(angleFilePath, i, numOfRays, k, a));

				simIndex++;

				double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

				auto end = std::chrono::high_resolution_clock::now();

				double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

				std::cout << "Completed Render : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;
			}

			j_angle[layerName] = filePaths;

			std::cout << "Completed Layer Count: " << i << std::endl;
		}

		j[angleName] = j_angle;
	}

	std::ofstream file("Simulations/Simulation2_AM15GSpectrum/FilePaths.json");
	file << j.dump(2);
	file.close();
}

void RunSimulationCategory3()
{
	CreateFolder("Simulations");
	CreateFolder("Simulations/Simulation3_NormalPerturbance");

	std::string filePath = "Simulations/Simulation3_NormalPerturbance/";

	int avg = 1; //5
	int numOfRays = 10000;
	int maxLayers = 100; //100
	int maxAngle = 60; //60
	int maxPerturbanceDev = 7;

	int angleStep = 20;
	int layerStep = 20; //5

	int totalRuns = ((maxAngle / angleStep) + 1) * ((maxLayers - 5) / layerStep + 1) * avg * maxPerturbanceDev;
	int simIndex = 0;

	json j;

	for (int a = 0; a <= maxAngle; a += angleStep)
	{
		json j_angle;

		std::cout << "Starting Angle: " << a << " degrees" << std::endl;

		std::string angleName = "Angle_" + std::to_string(a);

		std::string angleFilePath = filePath + angleName;

		CreateFolder(angleFilePath);

		for (int p = 0; p <= maxPerturbanceDev; p++)
		{
			json j_perturbance;

			std::string perturbanceName = "PerturbanceDev_" + std::to_string(p);

			std::string perturbanceFilePath = angleFilePath + "/" + perturbanceName;

			CreateFolder(perturbanceFilePath);

			for (int i = 5; i <= maxLayers; i += layerStep)
			{
				json j_layer;

				std::string layerName = "Layers_" + std::to_string(i);

				std::vector<std::string> filePaths = std::vector<std::string>();

				for (int k = 0; k < avg; k++)
				{
					auto start = std::chrono::high_resolution_clock::now();

					filePaths.push_back(RunNormalPerturbance(perturbanceFilePath, i, numOfRays, k, a, p));

					simIndex++;

					double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

					auto end = std::chrono::high_resolution_clock::now();

					double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

					std::cout << "Completed Render : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;
				}

				j_perturbance[layerName] = filePaths;

				std::cout << "Completed Layer Count: " << i << std::endl;
			}

			j_angle[perturbanceName] = j_perturbance;
		}

		j[angleName] = j_angle;
	}

	std::ofstream file("Simulations/Simulation3_NormalPerturbance/FilePaths.json");
	file << j.dump(2);
	file.close();
}

void RunSimulationCategory4()
{
	CreateFolder("Simulations");
	CreateFolder("Simulations/Simulation4_WavyNormalPerturbance");

	std::string filePath = "Simulations/Simulation4_WavyNormalPerturbance/";

	int avg = 1;//5
	int numOfRays = 10000;
	int maxLayers = 100; //100
	int maxAngle = 60; //60
	int maxPerturbanceDev = 7;

	int angleStep = 20;
	int layerStep = 20;//5

	int totalRuns = ((maxAngle / angleStep) + 1) * ((maxLayers - 5) / layerStep + 1) * avg * maxPerturbanceDev;
	int simIndex = 0;

	json j;

	for (int a = 0; a <= maxAngle; a += angleStep)
	{
		json j_angle;

		std::cout << "Starting Angle: " << a << " degrees" << std::endl;

		std::string angleName = "Angle_" + std::to_string(a);

		std::string angleFilePath = filePath + angleName;

		CreateFolder(angleFilePath);

		for (int p = 0; p <= maxPerturbanceDev; p++)
		{
			json j_perturbance;

			std::string perturbanceName = "PerturbanceDev_" + std::to_string(p);

			std::string perturbanceFilePath = angleFilePath + "/" + perturbanceName;

			CreateFolder(perturbanceFilePath);

			for (int i = 5; i <= maxLayers; i += layerStep)
			{
				json j_layer;

				std::string layerName = "Layers_" + std::to_string(i);

				std::vector<std::string> filePaths = std::vector<std::string>();

				for (int k = 0; k < avg; k++)
				{
					auto start = std::chrono::high_resolution_clock::now();

					filePaths.push_back(RunWavyNormalPerturbance(perturbanceFilePath, i, numOfRays, k, a, p));

					simIndex++;

					double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

					auto end = std::chrono::high_resolution_clock::now();

					double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

					std::cout << "Completed Render : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;
				}

				j_perturbance[layerName] = filePaths;

				std::cout << "Completed Layer Count: " << i << std::endl;
			}

			j_angle[perturbanceName] = j_perturbance;
		}

		j[angleName] = j_angle;
	}

	std::ofstream file("Simulations/Simulation4_WavyNormalPerturbance/FilePaths.json");
	file << j.dump(2);
	file.close();
}
