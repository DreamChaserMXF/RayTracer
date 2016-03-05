#pragma warning(disable:4996)

#include <iostream>
#include "GlobalTerm.h"

using std::cout;
using std::endl;
using std::string;

int main()
{
	const string pic_name = "RayTracer.png";
	freopen("output.txt", "w", stdout);
	string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\testscenes\\scene1.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\testscenes\\scene2.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\testscenes\\scene3.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene4-ambient.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene4-diffuse.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene4-emission.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene4-specular.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene5.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene6.test";
	//string scene_file = "D:\\MXFXF\\Project\\C++\\OpenGL\\CG Introduction\\RayTracer\\hw3-submissionscenes\\scene7.test";
	ContentParse(scene_file);
	//cout << G_CAM_LOOKFROM.z_ << endl;
	BYTE* pixels = RayTracer();
	SavePicture(pixels, pic_name);
	delete[] pixels;
	return 0;
}