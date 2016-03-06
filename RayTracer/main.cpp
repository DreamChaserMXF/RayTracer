#pragma warning(disable:4996)

#include <iostream>
#include "GlobalTerm.h"

using std::cout;
using std::endl;
using std::string;

int main()
{
	const string pic_name = "RayTracer.png";
	//freopen("output.txt", "w", stdout);
	ofstream out("output.txt");
	//string scene_file = ".\\testscenes\\scene1.test";
	//string scene_file = ".\\testscenes\\scene2.test";
	//string scene_file = ".\\testscenes\\scene3.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene4-ambient.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene4-diffuse.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene4-emission.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene4-specular.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene5.test";
	//string scene_file = ".\\hw3-submissionscenes\\scene6.test";
	string scene_file = ".\\hw3-submissionscenes\\scene7.test";
	cout << "parsing: ";
	ContentParse(scene_file, out);
	//cout << G_CAM_LOOKFROM.z_ << endl;
	cout << "ray tracing:" << endl;
	BYTE* pixels = RayTracer();
	SavePicture(pixels, pic_name);
	delete[] pixels;
	return 0;
}