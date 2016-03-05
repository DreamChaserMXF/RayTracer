#include "GlobalTerm.h"

BYTE* RayTracer()
{
	// 计算出相机坐标系三个坐标轴的表示矩阵mRotate
	Vector w = (G_CAM_LOOKAT - G_CAM_LOOKFROM).Normalize();
	Vector u = cross(G_CAM_UP, w).Normalize();
	Vector v = cross(w, u);
	Matrix mRotate(4);
	mRotate[0][0] = u.x_;
	mRotate[1][0] = u.y_;
	mRotate[2][0] = u.z_;
	mRotate[0][1] = v.x_;
	mRotate[1][1] = v.y_;
	mRotate[2][1] = v.z_;
	mRotate[0][2] = w.x_;
	mRotate[1][2] = w.y_;
	mRotate[2][2] = w.z_;
	mRotate[3][3] = 1.0;
	// 计算平移矩阵mTranslate
	Matrix mTranslate = TranslateMatrix(-G_CAM_LOOKFROM);
	// 计算世界坐标系到相机坐标系的转换矩阵mTransform
	// 求mTransform的逆，即将相机坐标系向量转换为世界坐标系向量的转换矩阵mCam2World
	Matrix mTransform = mRotate * mTranslate;
	cout << mTransform << endl;
	//Matrix mInverseTransform = mTransform.Inverse();
	// 一种新的求逆的方法
	mTranslate[0][3] = -mTranslate[0][3];
	mTranslate[1][3] = -mTranslate[1][3];
	mTranslate[2][3] = -mTranslate[2][3];
	Matrix mInverseTransform = mTranslate * mRotate.Tranpose();
	cout << mInverseTransform << endl;
	// 屏幕距相机的距离
	double distance = 1.0 / tan(xf::radians(G_FIELD_OF_VIEW / 2));
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// 用TeapotViewing那种方式去计算宽度是行不通的，不知道为什么
	for(int i = 0; i < G_HEIGHT; ++i)
	{
		for(int j = 0; j < G_WIDTH; ++j)
		{
			// 求出点(i,j)在相机坐标系的位置
			Vector position_in_cam((G_WIDTH / 2.0 - j) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0) / (G_HEIGHT / 2.0), distance);
			// 世界坐标系中的位置
			Vector position_in_global = mInverseTransform * position_in_cam;
			// 方向
			Vector direction = (position_in_global - G_CAM_LOOKFROM).Normalize();
			double tmin = (position_in_global - G_CAM_LOOKFROM).Length();
			double tmax = std::numeric_limits<double>::max();
			// 生成光线
			Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
			// 确定该点的颜色
			Vector color = TraceColor(ray);
			// 存储颜色
			//if(i < G_HEIGHT / 2 && j < G_WIDTH / 2)
			//{
			//	pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(0);
			//	pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(128);
			//	pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(255);
			//}
			//else
			//{
			//	pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(255);
			//	pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(128);
			//	pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(0);
			//}
			// 存储图片时，用BGR的格式
			pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(color.b_);
			pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(color.g_);
			pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(color.r_);
		}
	}
	return pixels;
}


Vector TraceColor(const Ray &ray)
{
	const Vector kBackGroundColor(128, 128, 255);
	enum {NONE, SPHERE, TRIANGLE} intersection_type = NONE;
	// 与物体求交
	double min_distance = std::numeric_limits<double>::max();
	// 与球求交
	list<Sphere>::const_iterator sphere_iter;
	double a = ray.direction_ * ray.direction_;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		double b = 2 * ray.direction_ * (ray.direction_ - c_iter->center_);
		double c = (ray.origin_ - c_iter->center_) * (ray.origin_ - c_iter->center_);
		double delta = b * b - 4 * a * c;
		if(abs(delta) < xf::EPS)	// 直线与球只有一个交点
		{
			// 计算交点的参数t
			double t = -b / 2.0 / a;
			if(t >= ray.tmin_ && t < min_distance)	// 线段与球相交，且交点更近
			{
				min_distance = t;
				intersection_type = SPHERE;
				// TODO 记下这个球的迭代器
				sphere_iter = c_iter;
			}
		}
		else if(delta > 0.0)	// 有两个交点，计算出两个t来
		{
			// t1 < t2
			double t1 = (-b - sqrt(delta)) / 2.0 / a;
			double t2 = (-b + sqrt(delta)) / 2.0 / a;
			// 如果t1符合要求
			if(t1 >= ray.tmin_ && t1 < min_distance)
			{
				min_distance = t1;
				intersection_type = SPHERE;
				// TODO 记下这个球的迭代器
				sphere_iter = c_iter;
			}
			else if(t2 >= ray.tmin_ && t2 < min_distance)
			{
				min_distance = t1;
				intersection_type = SPHERE;
				// TODO 记下这个球的迭代器
				sphere_iter = c_iter;
			}
		}
	}

	// 与三角形求交
	list<Triangle>::const_iterator triangle_iter;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		// 先判断直线的方向与三角形是否共面
		if(abs(cos(ray.direction_, c_iter->normal_)) > std::numeric_limits<double>::min())
		{
			// 不共面的情况
			// 则直线一定和三角形所在的平面相交
			// 求交点的参数t
			double t = ((c_iter->v1_ - ray.origin_) * c_iter->normal_) / (ray.direction_ * c_iter->normal_);
			if(t >= ray.tmin_ && t < min_distance)
			{
				// 求交点
				Vector intersection_point = ray.origin_ + t * ray.direction_;
				// 判断该点是否在三角形内
				// 有角度法、面积法、同向法、重心法等算法，这里采用面积法
				// 先计算三角形的面积
				double tri_aera = cross(c_iter->v2_ - c_iter->v1_, c_iter->v3_ - c_iter->v1_).Length() / 2.0;
				// 再计算以intersection_point为顶点的三个小三角形的面积
				double aera1 = cross(intersection_point - c_iter->v1_, intersection_point - c_iter->v2_).Length() / 2.0;
				double aera2 = cross(intersection_point - c_iter->v2_, intersection_point - c_iter->v3_).Length() / 2.0;
				double aera3 = cross(intersection_point - c_iter->v3_, intersection_point - c_iter->v1_).Length() / 2.0;
				// 比较看三个小三角形的面积和是否等于大三角形的面积
				if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// 相等，说明在里面
				{
					min_distance = t;
					intersection_type = TRIANGLE;
					// 记录指向该三角形的迭代器
					triangle_iter = c_iter;
				}
			}
		}
	}
	// 确定颜色
	Vector color;
	// 确定最近交点
	if(NONE == intersection_type)
	{
		color = kBackGroundColor;
	}
	else if(TRIANGLE == intersection_type)
	{
		// 计算此处的颜色
		color = triangle_iter->ambient_ + triangle_iter->material_.emission_;
		color *= 255;
	}
	else
	{
		assert(SPHERE == intersection_type);
		color = sphere_iter->ambient_ + sphere_iter->material_.emission_;
		color *= 255;
	}
	return color;
}