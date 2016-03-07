#pragma warning(disable:4996)

#include <iostream>
#include "GlobalTerm.h"

using std::cout;
using std::endl;
using std::string;

const char* scene_files[] = {
/*  0 */	 ".\\testscenes\\scene1-camera1.test",
/*  1 */	 ".\\testscenes\\scene1-camera2.test",
/*  2 */	 ".\\testscenes\\scene1-camera3.test",
/*  3 */	 ".\\testscenes\\scene1-camera4.test",
/*  4 */	 ".\\testscenes\\scene2-camera1.test",
/*  5 */	 ".\\testscenes\\scene2-camera2.test",
/*  6 */	 ".\\testscenes\\scene2-camera3.test",
/*  7 */	 ".\\testscenes\\scene3.test",
/*  8 */	 ".\\hw3-submissionscenes\\scene4-ambient.test",
/*  9 */	 ".\\hw3-submissionscenes\\scene4-diffuse.test",
/* 10 */	 ".\\hw3-submissionscenes\\scene4-emission.test",
/* 11 */	 ".\\hw3-submissionscenes\\scene4-specular.test",
/* 12 */	 ".\\hw3-submissionscenes\\scene5.test",	// thousand balls
/* 13 */	 ".\\hw3-submissionscenes\\scene6.test",	// cornell box
/* 14 */	 ".\\hw3-submissionscenes\\scene7.test",	// stanford dragon
};

static void RenderOneScene()
{
	ofstream out("output.txt");
	cout << "parsing: ";
	ContentParse(scene_files[14], out);
	//cout << G_CAM_LOOKFROM.z_ << endl;
	cout << "\nray tracing:" << endl;
	BYTE* pixels = Render();
	SavePicture(pixels, G_OUTPUT_FILENAME);
	delete[] pixels;
}

static void BatchRender()
{
	ofstream out("output.txt");
	int scene_num = sizeof(scene_files) / sizeof(scene_files[0]);
	for(int i = 0; i < scene_num; ++i)
	{
		cout << "num" << i + 1 << ":" << endl;
		init();
		cout << "parsing: ";
		ContentParse(scene_files[i], out);
		cout << "\nray tracing:" << endl;
		BYTE* pixels = Render();
		SavePicture(pixels, G_OUTPUT_FILENAME);
		delete[] pixels;
	}
	
}

int main()
{
	RenderOneScene();
	//BatchRender();
	return 0;
}