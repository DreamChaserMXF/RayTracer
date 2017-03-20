#include "GlobalTerm.h"

void SavePicture(BYTE* pixels, const string &name)
{
	FreeImage_Initialise();
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, G_WIDTH, G_HEIGHT, G_WIDTH * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, 0);	// 0表示第一个元素是图像的左下角
	FreeImage_Save(FIF_PNG, img, name.c_str(), 0);
	FreeImage_DeInitialise();
}