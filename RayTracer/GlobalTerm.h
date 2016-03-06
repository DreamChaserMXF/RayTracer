#ifndef GLOBALTERM_H
#define GLPBALTERM_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <vector>
#include <stack>
#include <cassert>
#include <climits>

#include <FreeImage.h>
#include "Primitive.h"
#include "Material.h"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Transform.hpp"

using xf::Matrix;
using xf::Vector;
using std::ostream;
using std::cout;
using std::endl;
using std::ends;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::list;
using std::vector;
using std::stack;

extern int G_HEIGHT;
extern int G_WIDTH;
extern int G_MAXDEPTH;
extern string G_OUTPUT_FILENAME;

extern Vector G_CAM_LOOKFROM;
extern Vector G_CAM_LOOKAT;
extern Vector G_CAM_UP;
extern double G_FIELD_OF_VIEW;

extern list<Light> G_POINTLIGHT_LIST;
extern list<Light> G_DIRECTIONALLIGHT_LIST;
extern Vector G_ATTENUATION;


extern list<Sphere> G_SPHERE_LIST;
extern list<Triangle> G_TRIANGLE_LIST;

extern stack<Matrix> G_MODELVIEWMATRIX_STACK;
extern Matrix G_CUR_TRANSFORM_MATRIX;


void ContentParse(const string &scene_file, ostream &out);
BYTE* RayTracer();
void SavePicture(BYTE* pixels, const string &name);

#endif