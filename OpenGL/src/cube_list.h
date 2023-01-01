#pragma once

#include "cube.h"
#include <vector>

class CubeList
{
public:
	CubeList();

	void AddCube(Cube* cube);

	void RemoveCube(Cube* cube);

	void RenderCubes();

	Cube* GetCubeByName(const std::string& name);

	std::vector<Cube*> Cubes;
private:
};