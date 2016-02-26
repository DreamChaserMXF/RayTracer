#ifndef MATERIAL_H
#define MATERIAL_H
#include "Vector.hpp"
struct Material
{
	xf::Vector<int> emission;
	xf::Vector<int> diffuse;
	int shininess;
	xf::Vector<int> specular;
};

#endif