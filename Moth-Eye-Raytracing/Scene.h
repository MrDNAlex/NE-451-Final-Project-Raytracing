#pragma once
#include <vector>
#include "Ray.h"
#include "Object.h"
#include "Segment.h"
#include <iostream>
#include "Frame.h"
#include <fstream>
#include "RaySource.h"
#include "Target.h"
#include <chrono>

class Scene
{
public:

	struct SceneStats
	{
	public:
		int StartRays = 0;
		double StartPower = 0.0;

		int LostRays = 0;
		int DestroyedRays = 0;
		double LostPower = 0.0;
		double DestroyedPower = 0.0;

		int CapturedRays = 0;
		double CapturedPower = 0.0;

		double InitializationTimeMS = 0.0;
		double RenderTimeMS = 0.0;
		double SaveTimeMS = 0.0;
		double AccumulationTimeMS = 0.0;

		int NumberOfFrames = 0;
		int NumberOfSegments = 0;

		std::string Name = "SceneStats";

		json ToJSON()
		{
			json j;
			j["StartRays"] = StartRays;
			j["StartPower"] = StartPower;
			j["LostRays"] = LostRays;
			j["DestroyedRays"] = DestroyedRays;
			j["LostPower"] = LostPower;
			j["DestroyedPower"] = DestroyedPower;
			j["CapturedRays"] = CapturedRays;
			j["CapturedPower"] = CapturedPower;
			j["InitializationTimeMS"] = InitializationTimeMS;
			j["RenderTimeMS"] = RenderTimeMS;
			j["SaveTimeMS"] = SaveTimeMS;
			j["AccumulationTimeMS"] = AccumulationTimeMS;
			j["NumberOfFrames"] = NumberOfFrames;
			j["NumberOfSegments"] = NumberOfSegments;
			j["TotalNumberOfRays"] = CapturedRays + DestroyedRays + LostRays;
			j["TotalSimTimeMS"] = InitializationTimeMS + RenderTimeMS + SaveTimeMS + AccumulationTimeMS;
			j["Name"] = Name;
			return j;
		}
	};

	std::vector<Object*> Objects;

	std::vector<Ray> Rays;

	std::vector<Frame> Frames;

	std::vector<RaySource*> RaySources;

	std::string FileName;

	SceneStats Stats;

	// Non-copyable
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	// Movable
	Scene(Scene&&) noexcept = default;
	Scene& operator=(Scene&&) noexcept = default;

	~Scene() {
		for (auto* p : Objects)    delete p;
		for (auto* p : RaySources) delete p;
	}

	Scene(std::string fileName)
	{
		this->Objects = std::vector<Object*>();
		this->Rays = std::vector<Ray>();
		this->Frames = std::vector<Frame>();
		this->FileName = fileName;
	}

	void AddObject(Object* object)
	{
		this->Objects.push_back(object);
	}

	void AddRay(Ray ray)
	{
		this->Rays.push_back(ray);
	}

	void AddRays(std::vector<Ray> rays)
	{
		this->Rays.reserve(this->Rays.size() + rays.size());
		this->Rays.insert(this->Rays.end(), rays.begin(), rays.end());
	}

	void AddFrame(Frame frame)
	{
		this->Frames.push_back(frame);
	}

	void AddRaySource(RaySource* source)
	{
		this->RaySources.push_back(source);
	}

	void Render(bool saveJSON = true, bool debug = true, bool saveAnimation = true)
	{
		this->Initialize(debug);	
		this->Bake(saveJSON, debug, saveAnimation);
		this->AccumulateStats();
		this->Save(saveJSON, debug, saveAnimation);
	}

	void Initialize(bool debug)
	{
		auto start = std::chrono::high_resolution_clock::now();

		if (debug)
		{
			std::cout << "Initializing Scene" << std::endl;
			std::cout << "Generating Rays from Sources" << std::endl;
		}

		for (int i = 0; i < this->RaySources.size(); i++)
		{
			RaySource* source = this->RaySources[i];
			std::vector<Ray> generatedRays = source->GenerateRays();
			this->AddRays(generatedRays);

			if (debug)
				std::cout << "Generated " << generatedRays.size() << " Rays from Source " << i << std::endl;
		}

		for (int j = 0; j < this->Objects.size(); j++)
		{
			if (debug)
				std::cout << "Building BVH for Object " << j << std::endl;

			this->Objects[j]->BVH();
		}

		double totalPower = 0.0;
		for (int i = 0; i < this->Rays.size(); i++)
		{
			this->Rays[i].Index = i;
			totalPower += this->Rays[i].Power;
		}

		Stats.StartRays = this->Rays.size();
		Stats.StartPower = totalPower;

		auto end = std::chrono::high_resolution_clock::now();
		Stats.InitializationTimeMS = std::chrono::duration<double, std::milli>(end - start).count();

		if (debug)
			std::cout << "Finished Initializing Scene in " << Stats.InitializationTimeMS << " ms" << std::endl;
	}

	void Bake(bool saveJSON = true, bool debug = true, bool saveAnimation = true)
	{
		auto start = std::chrono::high_resolution_clock::now();

		if (debug)
			std::cout << "Rendering Scene" << std::endl;

		int index = 0;

		while (this->Rays.size() > 0)
		{
			Frame frame = Frame(index);

			for (Ray& ray : this->Rays)
				frame.AddRay(ray);

			std::vector<Ray> newRays = std::vector<Ray>();

			newRays.reserve(this->Rays.size() * 2); // Estimate

			for (Ray& ray : this->Rays)
			{
				std::vector<Ray> traveledRays = this->Travel(&ray, &frame);

				if (traveledRays.size() > 0)
					newRays.insert(newRays.end(), traveledRays.begin(), traveledRays.end());
			}

			if (debug)
				std::cout << "Rendered Frame " << index << ": " << this->Rays.size() << " Rays, " << frame.DestroyedRays << " Destroyed, " << frame.LostRays << " Lost" << std::endl;

			AddFrame(frame);
			this->Rays = newRays;
			index++;
		}

		Frame frame = Frame(index);

		AddFrame(frame);

		auto end = std::chrono::high_resolution_clock::now();

		Stats.RenderTimeMS = std::chrono::duration<double, std::milli>(end - start).count();

		if (debug)
			std::cout << "Rendering Complete in " << Stats.RenderTimeMS << " ms" << std::endl;
	}

	void AccumulateStats()
	{
		auto start = std::chrono::high_resolution_clock::now();

		Stats.NumberOfFrames = this->Frames.size();
		Stats.Name = FileName;

		//Getting Stats
		for (int i = 0; i < this->Frames.size(); i++)
		{
			Stats.LostRays += this->Frames[i].LostRays;
			Stats.DestroyedRays += this->Frames[i].DestroyedRays;
			Stats.LostPower += this->Frames[i].LostPower;
			Stats.DestroyedPower += this->Frames[i].DestroyedPower;
		}

		for (int j = 0; j < this->Objects.size(); j++)
		{
			if (this->Objects[j]->Type == "Target")
			{
				Target* target = static_cast<Target*>(this->Objects[j]);

				Stats.CapturedPower += target->CapturedPower;
				Stats.CapturedRays += target->CapturedRays;
			}

			Stats.NumberOfSegments += this->Objects[j]->Segments.size();
		}

		auto end = std::chrono::high_resolution_clock::now();

		Stats.AccumulationTimeMS += std::chrono::duration<double, std::milli>(end - start).count();
	}

	void Save(bool saveJSON = true, bool debug = true, bool saveAnimation = true)
	{
		if (!saveJSON)
			return;

		if (debug)
			std::cout << "Saving..." << std::endl;

		auto startSave = std::chrono::high_resolution_clock::now();

		json j;

		j["Geometry"] = json::array();

		for (Object* object : this->Objects)
			j["Geometry"].push_back(object->ToJSON());

		if (saveAnimation)
		{
			j["Frames"] = json::array();

			for (Frame& frame : this->Frames)
				j["Frames"].push_back(frame.ToJSON());
		}
		else
			j["Frames"].push_back(this->Frames[0].ToJSON());

		if (debug)
			std::cout << "Converted To JSON, Dumping..." << std::endl;

		auto endSave = std::chrono::high_resolution_clock::now();

		Stats.SaveTimeMS += std::chrono::duration<double, std::milli>(endSave - startSave).count();

		j["Stats"] = Stats.ToJSON();

		//Save the File 
		std::ofstream file(FileName + ".json");
		file << j.dump(2);  // Pretty Indent of 4 Spaces
		file.close();

		if (debug)
			std::cout << "Render Saved" << std::endl;
	}

	std::vector<Ray> Travel(Ray* ray, Frame* frame)
	{
		ray->Bounce();

		if (ray->DestroyRay())
		{
			frame->DestroyedRays += 1;
			frame->DestroyedPower += ray->Power;
			return std::vector<Ray>();
		}

		double minT = INFINITY;
		Segment* closestSegment = nullptr;
		Object* closestObject = nullptr;

		for (Object* object : this->Objects)
		{
			RayHit hit = object->Intersect(ray);

			if (hit.Hit && hit.Distance < minT)
			{
				minT = hit.Distance;
				closestSegment = hit.SegmentHit;
				closestObject = object;
			}
		}

		if (closestSegment == nullptr || closestObject == nullptr)
		{
			frame->LostRays += 1;
			frame->LostPower += ray->Power;
			return std::vector<Ray>();
		}

		return closestObject->InteractWithRay(closestSegment, ray);
	}
};