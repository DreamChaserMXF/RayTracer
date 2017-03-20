#include "GlobalTerm.h"

int G_HEIGHT = 0;
int G_WIDTH = 0;
int G_MAXDEPTH = 5;
string G_OUTPUT_FILENAME = "RayTracer.png";

Vector G_CAM_LOOKFROM;
Vector G_CAM_LOOKAT;
Vector G_CAM_UP;
double G_FIELD_OF_VIEW = 0;

bool G_DIRECTIONAL_UNIVERSE = false;
bool G_COLOR_BLEEDING = false;

list<Light> G_POINTLIGHT_LIST;
list<Light> G_DIRECTIONALLIGHT_LIST;
Vector G_ATTENUATION(1.0, 0.0, 0.0);

list<Primitive*> G_PRIMITIVE_LIST;
list<Sphere> G_SPHERE_LIST;
list<Triangle> G_TRIANGLE_LIST;

stack<Matrix> G_MODELVIEWMATRIX_STACK;
Matrix G_CUR_TRANSFORM_MATRIX = IdentityMatrix(4);

void init()
{
	G_HEIGHT = 0;
	G_WIDTH = 0;
	G_MAXDEPTH = 5;
	G_OUTPUT_FILENAME = "RayTracer.png";
	G_FIELD_OF_VIEW = 0;
	G_ATTENUATION = Vector(1.0, 0.0, 0.0);
	G_CUR_TRANSFORM_MATRIX = IdentityMatrix(4);

	G_POINTLIGHT_LIST.clear();
	G_DIRECTIONALLIGHT_LIST.clear();
	G_SPHERE_LIST.clear();
	G_TRIANGLE_LIST.clear();
	while(!G_MODELVIEWMATRIX_STACK.empty())
	{
		G_MODELVIEWMATRIX_STACK.pop();
	}
}