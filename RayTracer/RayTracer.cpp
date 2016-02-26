#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <stack>
#include <FreeImage.h>
#include "Vector.hpp"
#include "Matrix.hpp"

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

struct Sphere
{
	int x;
	int y;
	int z;
	int radius;
};
struct Triangle
{
	int x;
	int y;
	int z;
};

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
		buffer.clear();		// 不知道为什么，一定要加这个，否则camera那行就会读不进去
		buffer.str(line);
		buffer >> command;
		if("size" == command)
		{
			//cout << line << endl;
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
			//cout << line << endl;
			buffer >> G_CAM_LOOKFROM_X >> G_CAM_LOOKFROM_Y >> G_CAM_LOOKFROM_Z
				   >> G_CAM_LOOKAT_X   >> G_CAM_LOOKAT_Y   >> G_CAM_LOOKAT_Z
				   >> G_CAM_UP_X       >> G_CAM_UP_Y       >> G_CAM_UP_Z
				   >> G_FIELD_OF_VIEW;
		}
		else if("sphere" == command)
		{
			buffer >> sphere.x >> sphere.y >> sphere.z >> sphere.radius;
			// TODO save transformed sphere
		}
		else if("tri" == command)
		{
			buffer >> triangle.x >> triangle.y >> triangle.z;
			// TODO save transformed triangle
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
			//G_MODELVIEWMATRIX_STACK.push(translate_mat);
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
			//G_MODELVIEWMATRIX_STACK.push(translate_mat);
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
			
		}
		else if("specular" == command)
		{
			
		}
		else if("shininess" == command)
		{
			
		}
		else if("emission" == command)
		{
			
		}
		else if("maxverts" == command)
		{
		
		}
		else if("maxvertnorms" == command)
		{
		
		}
		//else if("maxverts" == command)
		//{
		//
		//}
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
		 << "camera field of view: " << G_FIELD_OF_VIEW << endl;
}

int main()
{
	string scene_file = "D:\\MXFXF\\Temporary Files\\hw3-submissionscenes\\scene4-ambient.test";
	ContentParse(scene_file);
	
	return 0;
}