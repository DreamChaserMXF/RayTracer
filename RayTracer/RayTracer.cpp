#include "GlobalTerm.h"
#include <ctime>



static bool IsVisible(const Ray &ray)
{
	double t = 0.0;
	// 与球求交
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			return false;
		}
	}
	// 与三角形求交
	list<Triangle>::const_iterator triangle_iter;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			return false;
		}
	}
	return true;
}


static Vector ComputeLight(const Vector &eye, const Vector &vertex, const Vector &normal, const Material &material)
{
	Vector color;
	// 点光源
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		// 先计算点光源对该点是否可见
		Vector diff_vec = c_iter->origin_ - vertex;
		Vector light_direction = diff_vec.GetNormalize();	// imp! GetXXX
		double distance = diff_vec.Length();
		if(distance > DBL_MIN)	// 光源与交点之间一定要有距离才行
		{
			Vector half_vec = ((eye - vertex).Normalize() + light_direction).Normalize();
			if(half_vec.Length() > DBL_MIN)	// 光线与视线不能共线反向
			{
				Ray light_ray(vertex, light_direction, xf::EPS, distance);	// imp！ EPS而不是0，如果tmin为0的话，那么，起始点就一定会与得到该交点的那个三角形相交
				if(IsVisible(light_ray))	// 可见
				{
					Vector point_color = dot_cross(c_iter->color_ / (G_ATTENUATION.x_ + G_ATTENUATION.y_ * distance + G_ATTENUATION.z_ * distance * distance),	// 衰减系数
								(material.diffuse_ 
								* xf::max(normal * light_direction, 0.0))		                               // 漫射光
							+ (material.specular_ 
								* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // 反射光
						);	
					color += point_color;
				}
			}
		}
	}
	// 方向光
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
	c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		// 先计算点光源对该点是否可见
		Vector light_direction = (c_iter->origin_).GetNormalize();
		Vector half_vec = ((eye - vertex).Normalize() + light_direction).Normalize();
		if(half_vec.Length() > DBL_MIN)	// 光线与视线不能共线反向
		{
			Ray light_ray(vertex, light_direction, xf::EPS, DBL_MAX);	// imp！ EPS而不是0，如果tmin为0的话，那么，起始点就一定会与得到该交点的那个三角形相交
			if(IsVisible(light_ray))	// 可见
			{
				Vector directional_color = dot_cross(c_iter->color_,	// 衰减系数
							(material.diffuse_ 
							* xf::max(normal * light_direction, 0.0))		                               // 漫射光
						+ (material.specular_ 
							* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // 反射光
					);	
				color += directional_color;
			}
		}
	}
	return color;
}

static Vector TraceRay(int depth, const Ray &ray)
{
	if(depth > G_MAXDEPTH)
	{
		return Vector();
	}
	Primitive intersection_type = NONE;
	// 与物体求交
	double min_distance = ray.tmax_;
	// 与球求交
	double t = 0.0;
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t) && t < min_distance)
		{
			min_distance = t;
			intersection_type = SPHERE;
			sphere_iter = c_iter;
		}
	}
	// 与三角形求交
	list<Triangle>::const_iterator triangle_iter;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t) && t < min_distance)
		{
			min_distance = t;
			intersection_type = TRIANGLE;
			triangle_iter = c_iter;
		}
	}
	// 确定颜色
	Vector color;
	// 确定最近交点
	if(TRIANGLE == intersection_type)
	{
		// 计算此处的颜色
		// 环境光+自发光
		color = triangle_iter->ambient_ + triangle_iter->material_.emission_;
		// 计算交点位置
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// 计算光照
		color += ComputeLight(ray.origin_, intersection_point, triangle_iter->normal_, triangle_iter->material_);
	}
	else if(SPHERE == intersection_type)
	{
		assert(SPHERE == intersection_type);
		color = sphere_iter->ambient_ + sphere_iter->material_.emission_;
		// 计算交点位置
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// 计算交点法向
		// 1. 把交点变换回原位置 2. 求出原位置的法向 3. 把法向变换到最终方向
		Vector original_point = sphere_iter->inv_transform_mat_ * intersection_point;
		Vector original_normal = (original_point - sphere_iter->center_).Normalize();
		Vector normal = sphere_iter->inv_transform_mat_.GetTranspose().TransformDirection(original_normal).Normalize();
		// 计算光照
		color += ComputeLight(ray.origin_, intersection_point, normal, sphere_iter->material_);
	}
	return color;
}

BYTE* RayTracer()
{
	// 计算出相机坐标系三个坐标轴的表示矩阵mRotate
	Vector w = (G_CAM_LOOKFROM - G_CAM_LOOKAT).Normalize();
	Vector u = cross(G_CAM_UP, w).Normalize();
	Vector v = cross(w, u);
	Matrix mRotate(4);
	// 这里的行列千万别搞反了
	mRotate[0][0] = u.x_;
	mRotate[0][1] = u.y_;
	mRotate[0][2] = u.z_;
	mRotate[1][0] = v.x_;
	mRotate[1][1] = v.y_;
	mRotate[1][2] = v.z_;
	mRotate[2][0] = w.x_;
	mRotate[2][1] = w.y_;
	mRotate[2][2] = w.z_;
	mRotate[3][3] = 1.0;
	// 计算平移矩阵mTranslate
	Matrix mTranslate = TranslateMatrix(-G_CAM_LOOKFROM);
	// 计算世界坐标系到相机坐标系的转换矩阵mTransform
	// 求mTransform的逆，即将相机坐标系向量转换为世界坐标系向量的转换矩阵mCam2World
	Matrix mTransform = mRotate * mTranslate;
	//cout << mTransform << endl;
	//Matrix mInverseTransform = mTransform.GetInverse();
	// 一种新的求逆的方法
	mTranslate[0][3] = -mTranslate[0][3];
	mTranslate[1][3] = -mTranslate[1][3];
	mTranslate[2][3] = -mTranslate[2][3];
	Matrix mInverseTransform = mTranslate * mRotate.GetTranspose();
	//cout << mInverseTransform << endl;
	// 预先计算量
	double tmin = 0;
	//double tmin = (position_in_global - G_CAM_LOOKFROM).Length();
	double tmax = std::numeric_limits<double>::max();	// 射出的第一道光线是无穷远的(对光源的光线是有限长度的线段)
	double tan_fov_d2 = tan(xf::radians(G_FIELD_OF_VIEW / 2.0));
	// 存储像素的数组
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// 用TeapotViewing那种方式去计算宽度是行不通的，不知道为什么
	// 用于计算进度的变量
	int one_percent = G_HEIGHT / 100;
	int percentage = 0;
	time_t current_t = time(NULL);
	time_t last_t = current_t;
	int dot_index = 0;
	const char* dotclear_str[] = {"\b \b", "\b\b  \b\b", "\b\b\b   \b\b\b"};
	const char* dot_str[] = {".", "..", "..."};
	cout << "0%.";
	Vector color;
	for(int i = 0; i < G_HEIGHT; ++i)
	{
		if(0 == i % (one_percent))
		{
			percentage = i / one_percent;
			if(percentage >= 100)
			{
				percentage = 99;
			}
			cout << "\r                       \r";
			cout << "\t" << percentage << "%" << dot_str[dot_index];
		}
		current_t = time(NULL);
		if(current_t > last_t)
		{
			last_t = current_t;
			cout << dotclear_str[dot_index];
			dot_index = (dot_index + 1) % 3;
			cout << dot_str[dot_index];
		}
		for(int j = 0; j < G_WIDTH; ++j)
		{
			// 求出点(i,j)在相机坐标系的位置
			//Vector position_in_cam((j - G_WIDTH / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), -1.0);	// 原算式
			Vector position_in_cam((2.0 * j - G_WIDTH) * tan_fov_d2 / G_HEIGHT, (2.0 * i / G_HEIGHT - 1.0) * tan_fov_d2, -1.0);	// 化简后的算式
			// 世界坐标系中的位置
			Vector position_in_global = mInverseTransform * position_in_cam;
			// 方向
			Vector direction = (position_in_global - G_CAM_LOOKFROM).Normalize();
			if(direction.Length() > DBL_MIN)	// 判断一下，距离不能太近 
			{
				// 生成光线
				Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
				// 确定该点的颜色
				color = TraceRay(0, ray);
			}
			else
			{
				color.x_ = color.y_ = color.z_ = 0.0;
			}
			// 存储图片颜色时，需要用BGR的格式
			// 且需要将双精度的颜色转换为BYTE格式的颜色
			pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(color.b_ * 255.0);
			pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(color.g_ * 255.0);
			pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(color.r_ * 255.0);
		}
	}
	cout << "\r                            \r\t100%\n";
	return pixels;
}