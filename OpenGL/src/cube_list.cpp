#include "cube_list.h"

#include <iostream>

CubeList::CubeList()
{

}

void CubeList::AddCube(Cube* cube)
{
	Cubes.push_back(cube);
}

void CubeList::RemoveCube(Cube* cube)
{
	auto it = std::find(Cubes.begin(), Cubes.end(), cube);
	if (it != Cubes.end())
		Cubes.erase(it);
}

void CubeList::RenderCubes()
{
	for (auto& cube : Cubes)
	{
		if (cube->GetName() == "StrongCube")
		{
			cube->Render(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f));
		}
		else 
			cube->Render();
	}
}

Cube* CubeList::GetCubeByName(const std::string& name)
{
	for (auto& cube : Cubes)
	{
		if (cube->GetName() == name)
		{
			return cube;
		}
	}
	return nullptr;
}
