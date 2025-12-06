#pragma once
#include <vector>
#include <filesystem>
#include <string>
#include <iostream>
#include <direct.h>   
#include <io.h>       

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

bool DirectoryExists(const std::string& path)
{
	return _access(path.c_str(), 0) == 0;
}

void CreateFolder(const std::string& path)
{
	if (DirectoryExists(path))
		return;

	if (_mkdir(path.c_str()) != 0)
		std::cout << "Failed to create directory.\n";
}
