#include "GlobalTerm.h"

int G_HEIGHT = 0;
int G_WIDTH = 0;
int G_MAXDEPTH = 5;
string G_OUTPUT_FILENAME;

Vector G_CAM_LOOKFROM;
Vector G_CAM_LOOKAT;
Vector G_CAM_UP;
double G_FIELD_OF_VIEW = 0;

list<Light> G_POINTLIGHT_LIST;
list<Light> G_DIRECTIONALLIGHT_LIST;
Vector G_ATTENUATION(1.0, 0.0, 0.0);

list<Sphere> G_SPHERE_LIST;
list<Triangle> G_TRIANGLE_LIST;

stack<Matrix> G_MODELVIEWMATRIX_STACK;
Matrix G_CUR_TRANSFORM_MATRIX = IdentityMatrix(4);