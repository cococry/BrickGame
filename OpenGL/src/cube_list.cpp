#include "cube_list.h"

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
		cube->Render();
	}
}
