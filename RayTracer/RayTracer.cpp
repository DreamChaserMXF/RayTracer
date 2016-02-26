#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <stack>
#include <FreeImage.h>
#include "Primitive.h"
#include "Material.h"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Transform.hpp"
using xf::Matrix;
using xf::Vector;

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::list;
using std::stack;

int G_HEIGHT = 0;
int G_WIDTH = 0;
int G_MAXDEPTH = 5;
string G_OUTPUT_FILENAME;

int G_CAM_LOOKFROM_X = 0;
int G_CAM_LOOKFROM_Y = 0;
int G_CAM_LOOKFROM_Z = 0;
int G_CAM_LOOKAT_X = 0;
int G_CAM_LOOKAT_Y = 0;
int G_CAM_LOOKAT_Z = 0;
int G_CAM_UP_X = 0;
int G_CAM_UP_Y = 0;
int G_CAM_UP_Z = 0;
double G_FIELD_OF_VIEW = 0;


Material G_MATERIAL;
list<Sphere> G_SPHERE_LIST;
list<Triangle> G_TRIANGLE_LIST;
stack<Matrix<double>> G_MODELVIEWMATRIX_STACK;
Matrix<double> G_CUR_TRANSFORM_MATRIX = Matrix<double>::IdentityMatrix(4);


void ContentParse(const string &scene_file)
{
	string line;
	ifstream f_in(scene_file);
	if(!f_in)
	{
		perror("scene_file open error: ");
		exit(EXIT_FAILURE);
	}

	istringstream buffer;
	string command;
	Sphere sphere;
	Triangle triangle;
	while(std::getline(f_in, line))
	{
		//cout << line << endl;
		if(0 == line.size()
			|| '#' == line.at(0))
		{
			continue;
		}
		buffer.clear();		// ��֪��Ϊʲô��һ��Ҫ�����������camera���оͻ������ȥ
		buffer.str(line);
		buffer >> command;
		if("size" == command)
		{
			buffer >> G_WIDTH >> G_HEIGHT;
		}
		else if("maxdepth" == command)
		{
			buffer >> G_MAXDEPTH;
		}
		else if("output" == command)
		{
			buffer >> G_OUTPUT_FILENAME;
		}
		else if("camera" == command)
		{
			buffer >> G_CAM_LOOKFROM_X >> G_CAM_LOOKFROM_Y >> G_CAM_LOOKFROM_Z
				   >> G_CAM_LOOKAT_X   >> G_CAM_LOOKAT_Y   >> G_CAM_LOOKAT_Z
				   >> G_CAM_UP_X       >> G_CAM_UP_Y       >> G_CAM_UP_Z
				   >> G_FIELD_OF_VIEW;
		}
		else if("sphere" == command)
		{
			buffer >> sphere.x >> sphere.y >> sphere.z >> sphere.radius;
			G_SPHERE_LIST.push_back(Transform(G_CUR_TRANSFORM_MATRIX, sphere));
		}
		else if("tri" == command)
		{
			buffer >> triangle.x >> triangle.y >> triangle.z;
			G_TRIANGLE_LIST.push_back(Transform(G_CUR_TRANSFORM_MATRIX, triangle));
		}
		else if("vertex" == command)
		{
			
		}
		else if("translate" == command)
		{
			Vector<double> translate_vec;
			buffer >> translate_vec.x_ >> translate_vec.y_ >> translate_vec.z_;
			Matrix<double> translate_mat = Matrix<double>::TranslateMatrix(translate_vec);
			cout << "translation" << endl;
			cout << translate_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * translate_mat;
		}
		else if("rotate" == command)
		{
			Vector<double> rotation_vec;
			double degrees;
			buffer >> rotation_vec.x_ >> rotation_vec.y_ >> rotation_vec.z_ >> degrees;
			Matrix<double> rotation_mat = Matrix<double>::RotationMatrix(rotation_vec, degrees);
			cout << "rotation" << endl;
			cout << rotation_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * rotation_mat;
		}
		else if("scale" == command)
		{
			Vector<double> scale_vec;
			buffer >> scale_vec.x_ >> scale_vec.y_ >> scale_vec.z_;
			Matrix<double> scale_mat = Matrix<double>::ScaleMatrix(scale_vec);
			cout << "scale" << endl;
			cout << scale_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * scale_mat;
		}
		else if("pushTransform" == command)
		{
			G_MODELVIEWMATRIX_STACK.push(G_CUR_TRANSFORM_MATRIX);
			cout << "after push, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << endl;
		}
		else if("popTransform" == command)
		{
			G_CUR_TRANSFORM_MATRIX = G_MODELVIEWMATRIX_STACK.top();
			G_MODELVIEWMATRIX_STACK.pop();
			cout << "after pop, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << endl;
		}
		else if("directional" == command)
		{
			
		}
		else if("point" == command)
		{
			
		}
		else if("attenuation" == command)
		{
			
		}
		else if("ambient" == command)
		{
			
		}
		else if("diffuse" == command)
		{
			buffer >> G_MATERIAL.diffuse.r_ >> G_MATERIAL.diffuse.g_ >> G_MATERIAL.diffuse.b_;
		}
		else if("specular" == command)
		{
			buffer >> G_MATERIAL.specular.r_ >> G_MATERIAL.specular.g_ >> G_MATERIAL.specular.b_;
		}
		else if("shininess" == command)
		{
			buffer >> G_MATERIAL.shininess;
		}
		else if("emission" == command)
		{
			buffer >> G_MATERIAL.emission.r_ >> G_MATERIAL.emission.g_ >> G_MATERIAL.emission.b_;
		}
		else if("maxverts" == command)
		{
		
		}
		else if("maxvertnorms" == command)
		{
		
		}
		else
		{
			cout << "havn't handle this: " << line << endl;
		}
	}
	cout << "widht = " << G_WIDTH << "\theight = " << G_HEIGHT << endl;
	cout << "max depth = " << G_MAXDEPTH << endl;
	cout << "result file name:     " << G_OUTPUT_FILENAME << endl;
	cout << "camera look from:     " << G_CAM_LOOKFROM_X << ", " << G_CAM_LOOKFROM_Y << ", " << G_CAM_LOOKFROM_Z << endl
		 << "camera look at:       " << G_CAM_LOOKAT_X   << ", " << G_CAM_LOOKAT_Y   << ", " << G_CAM_LOOKAT_Z << endl
		 << "camera up direction:  " << G_CAM_UP_X       << ", " << G_CAM_UP_Y       << ", " << G_CAM_UP_Z << endl
		 << "camera field of view: " << G_FIELD_OF_VIEW << endl
		 << "material:" << endl
		 << "diffuse: " << G_MATERIAL.diffuse.r_ << ", " << G_MATERIAL.diffuse.g_ << ", " << G_MATERIAL.diffuse.b_ << endl
		 << "specular: " << G_MATERIAL.specular.r_ << ", " << G_MATERIAL.specular.g_ << ", " << G_MATERIAL.specular.b_ << endl
		 << "shininess: " << G_MATERIAL.shininess << endl
		 << "emission: " << G_MATERIAL.emission.r_ << ", " << G_MATERIAL.emission.g_ << ", " << G_MATERIAL.emission.b_ << endl;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		cout << c_iter->x << ", " << c_iter->y << ", " << c_iter->z << ", " << c_iter->radius << endl;
	}
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		cout << c_iter->x << ", " << c_iter->y << ", " << c_iter->z << endl;
	}
}

int main()
{
	string scene_file = "D:\\MXFXF\\Temporary Files\\hw3-submissionscenes\\scene4-ambient.test";
	ContentParse(scene_file);
	
	return 0;
}