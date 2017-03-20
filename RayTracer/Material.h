#ifndef MATERIAL_H
#define MATERIAL_H
#include "Vector.h"
using xf::Vector;
class Material
{
public:
	Material() : ambient_(0.2, 0.2, 0.2), emission_(), diffuse_(), shininess_(0), specular_(), mirror_coefficient_(), refraction_coefficient_(), refraction_index_(0.0)
	{}
	Vector ambient_;
	Vector emission_;
	Vector diffuse_;
	int shininess_;
	Vector specular_;
	Vector mirror_coefficient_;
	Vector refraction_coefficient_;
	double refraction_index_;
};

#endif