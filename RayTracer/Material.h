#ifndef MATERIAL_H
#define MATERIAL_H
#include "Vector.hpp"
using xf::Vector;
class Material
{
public:
	Material() : emission_(), diffuse_(), shininess_(0), specular_()
	{}
	Vector emission_;
	Vector diffuse_;
	int shininess_;
	Vector specular_;
};

#endif