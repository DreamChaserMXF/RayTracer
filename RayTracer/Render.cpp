#include "GlobalTerm.h"
#include <ctime>



static bool IsVisible(const Ray &ray, Vector &refraction_coef)
{
	double t = 0.0;
	//refraction_coef.x_ = refraction_coef.y_ = refraction_coef.z_ = 1.0;
	// 与球求交
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			if(c_iter->material_.refraction_coefficient_.Length() > DBL_MIN)
			{
				// 对球的交，乘的系数要小一些，因为会交两次
				refraction_coef = scale_product(refraction_coef, c_iter->material_.refraction_coefficient_ + (Vector(1.0, 1.0, 1.0) - c_iter->material_.refraction_coefficient_) / 2.0);
			}
			else
			{
				return false;
			}
		}
	}
	// 与三角形求交
	list<Triangle>::const_iterator triangle_iter;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			if(c_iter->material_.refraction_coefficient_.Length() > DBL_MIN)
			{
				refraction_coef = scale_product(refraction_coef, c_iter->material_.refraction_coefficient_);
				//refraction_coef = scale_product(refraction_coef, c_iter->material_.refraction_coefficient_ + (Vector(1.0, 1.0, 1.0) - c_iter->material_.refraction_coefficient_) / 2.0);
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

//static Vector TraceRay(int depth, const Ray &ray);

static Vector ComputeLight(int depth, const Vector &eye, const Vector &vertex, const Vector &normal, const Material &material, bool handle_refraction = true)
{
	// 初始化光照
	Vector color;
	// 视点与顶点重合
	Vector view_vec = eye - vertex;
	if(view_vec.Length() < DBL_MIN)
	{
		return color;
	}
	view_vec.Normalize();

	// 点光源
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		// 先计算点光源对该点是否可见
		Vector diff_vec = c_iter->origin_ - vertex;
		// 如果点与光源重合
		double distance = diff_vec.Length();
		if(distance > DBL_MIN)	// 光源与交点之间一定要有距离才行
		{
			Vector light_direction = diff_vec.GetNormalize();	// 有距离时，就可以归一化
			Vector half_vec = view_vec + light_direction;
			if(half_vec.Length() > DBL_MIN)	// 光线与视线不能共线反向
			{
				half_vec.Normalize();
				Ray light_ray(vertex, light_direction, xf::EPS, distance);	// imp！ EPS而不是0，如果tmin为0的话，那么，起始点就一定会与得到该交点的那个三角形相交
				Vector refraction_coef(1.0, 1.0, 1.0);
				if(IsVisible(light_ray, refraction_coef))	// 可见
				{
					Vector point_color = scale_product(c_iter->color_ / (G_ATTENUATION.x_ + G_ATTENUATION.y_ * distance + G_ATTENUATION.z_ * distance * distance),	// 衰减系数
								(material.diffuse_ 
								* xf::max(normal * light_direction, 0.0))		                               // 漫射光
							+ (material.specular_ 
								* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // 反射光
						);	
					color += scale_product(refraction_coef, point_color);	// refraction_coef是中间物体的折射系数
				}
			}
		}
	}
	// 方向光
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
	c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		Vector half_vec = view_vec + c_iter->origin_;
		if(half_vec.Length() > DBL_MIN)	// 光线与视线不能共线反向
		{
			half_vec.Normalize();
			Ray light_ray(vertex, c_iter->origin_, xf::EPS, DBL_MAX);	// imp！ EPS而不是0，如果tmin为0的话，那么，起始点就一定会与得到该交点的那个三角形相交
			Vector refraction_coef(1.0, 1.0, 1.0);
			if(G_DIRECTIONAL_UNIVERSE || IsVisible(light_ray, refraction_coef))	// 可见
			{
				Vector directional_color = scale_product(c_iter->color_,
							(material.diffuse_ 
							* xf::max(normal * c_iter->origin_, 0.0))	// 漫射光
						+ (material.specular_ 
							* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // 反射光
					);	
				color += scale_product(refraction_coef, directional_color);	// refraction_coef是中间物体的折射系数
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
	// 求交
	Primitive intersection_type = NONE;
	double min_distance = ray.tmax_;
	double t = 0.0;
	// 与球求交
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
		// 环境光+自发光
		color = triangle_iter->material_.ambient_ + triangle_iter->material_.emission_;
		// 计算交点位置
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// 计算直接光照
		color += ComputeLight(depth, ray.origin_, intersection_point, triangle_iter->normal_, triangle_iter->material_);
		// 计算反射光
		Vector reflection_direction = ray.direction_ - 2.0 * triangle_iter->normal_ * ray.direction_ * triangle_iter->normal_;
		if(triangle_iter->material_.mirror_coefficient_.Length() > DBL_MIN)
		{
			Ray reflection(intersection_point, reflection_direction, xf::EPS, DBL_MAX);
			color += scale_product(triangle_iter->material_.mirror_coefficient_, TraceRay(depth + 1, reflection));
		}
		// 渗色光
		//double weight = 1.0;
		if(G_COLOR_BLEEDING && 0 == depth)
		{
			for(int i = 0; i < 10; ++i)
			{
				Vector bleeding_coefficient(0.02, 0.02, 0.02);
				Vector arbitrary_shift((double)rand() / (double)RAND_MAX - 0.5, (double)rand() / (double)RAND_MAX - 0.5, (double)rand() / (double)RAND_MAX - 0.5);
				//Vector arbitrary_shift((double)rand() / (double)RAND_MAX / 2.0 - 0.25 , (double)rand() / (double)RAND_MAX / 2.0 - 0.25, (double)rand() / (double)RAND_MAX / 2.0 - 0.25);
				Vector bleeding_direction = (reflection_direction + arbitrary_shift);
				if(bleeding_direction.Length() > 0)
				{
					bleeding_direction.Normalize();
					double attenuation = cos(reflection_direction, bleeding_direction);
					Ray bleeding(intersection_point, bleeding_direction, xf::EPS, 0.5);
					color += scale_product(attenuation * bleeding_coefficient, TraceRay(G_MAXDEPTH, bleeding));
					//weight += 0.02 * attenuation;
				}
			}
			//color /= weight;
		}
	}
	else if(SPHERE == intersection_type)
	{
		// 环境光+自发光
		color = sphere_iter->material_.ambient_ + sphere_iter->material_.emission_;
		// 计算交点位置
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// 计算交点法向
		// 1. 把交点变换回原位置 2. 求出原位置的法向 3. 把法向变换到最终方向
		Vector original_point = sphere_iter->inv_transform_mat_ * intersection_point;
		Vector original_normal = (original_point - sphere_iter->center_).Normalize();	// parse的时候检查过了，球的半径不可能为0，故一定不会有异常
		Vector normal = sphere_iter->inv_transform_mat_.GetTranspose().TransformDirection(original_normal).Normalize();	// 方向变换之后，也一定不会为0向量
		// 计算直接光照
		color += ComputeLight(depth, ray.origin_, intersection_point, normal, sphere_iter->material_);
		// 计算折射光
		Vector mirror_gain;	// 入射角过大带来的全反射现象导致的反射系数增益
		if(sphere_iter->material_.refraction_coefficient_.Length() > DBL_MIN)
		{
			if(ray.direction_ * normal < 0.0)
			{
				Vector axis = cross_product(normal, ray.direction_);
				double refraction_angle = asin(axis.Length() / sphere_iter->material_.refraction_index_);
				if(refraction_angle < xf::PI / 2.0)
				{
					Matrix rotate_mat = RotateMatrixR(axis, xf::PI - refraction_angle);
					Ray refraction(intersection_point, rotate_mat.TransformDirection(normal), xf::EPS, DBL_MAX);
					color += scale_product(sphere_iter->material_.refraction_coefficient_, TraceRay(depth + 1, refraction));
				}
				else
				{
					mirror_gain = sphere_iter->material_.refraction_coefficient_;
				}
			}
			else
			{
				Vector axis = cross_product(normal, ray.direction_);
				double refraction_angle = asin(axis.Length() * sphere_iter->material_.refraction_index_);
				if(refraction_angle < xf::PI / 2.0)
				{
					Matrix rotate_mat = RotateMatrixR(axis, refraction_angle);
					Ray refraction(intersection_point, rotate_mat.TransformDirection(normal), xf::EPS, DBL_MAX);
					color += scale_product(sphere_iter->material_.refraction_coefficient_, TraceRay(depth + 1, refraction));
				}
				else
				{
					mirror_gain = sphere_iter->material_.refraction_coefficient_;
				}
			}
		}
		// 计算反射光
		Vector reflection_direction = ray.direction_ - 2.0 * normal * ray.direction_ * normal;
		mirror_gain += sphere_iter->material_.mirror_coefficient_;
		if(mirror_gain.Length() > DBL_MIN)
		{
			Ray reflection(intersection_point, reflection_direction, xf::EPS, DBL_MAX);
			color += scale_product(mirror_gain, TraceRay(depth + 1, reflection));
		}
		//if(sphere_iter->material_.mirror_coefficient_.Length() > DBL_MIN)
		//{
		//	Ray reflection(intersection_point, reflection_direction, xf::EPS, DBL_MAX);
		//	color += scale_product(sphere_iter->material_.mirror_coefficient_, TraceRay(depth + 1, reflection));
		//}

		// 渗色光
		//double weight = 1.0;
		if(G_COLOR_BLEEDING && 0 == depth)
		{
			for(int i = 0; i < 10; ++i)
			{
				Vector bleeding_coefficient(0.02, 0.02, 0.02);
				Vector arbitrary_shift((double)rand() / (double)RAND_MAX - 0.5, (double)rand() / (double)RAND_MAX - 0.5, (double)rand() / (double)RAND_MAX - 0.5);
				//Vector arbitrary_shift((double)rand() / (double)RAND_MAX / 2.0 - 0.25 , (double)rand() / (double)RAND_MAX / 2.0 - 0.25, (double)rand() / (double)RAND_MAX / 2.0 - 0.25);
				Vector bleeding_direction = (reflection_direction + arbitrary_shift);
				if(bleeding_direction.Length() > 0)
				{
					bleeding_direction.Normalize();
					double attenuation = cos(reflection_direction, bleeding_direction);
					Ray bleeding(intersection_point, bleeding_direction, xf::EPS, 0.5);
					color += scale_product(attenuation * bleeding_coefficient, TraceRay(G_MAXDEPTH, bleeding));
					//weight += 0.02 * attenuation;
				}
			}
			//color /= weight;
		}
	}
	return color;
}

BYTE* Render()
{
	// 计算出相机坐标系三个坐标轴的表示矩阵mRotate
	Vector w = (G_CAM_LOOKFROM - G_CAM_LOOKAT);
	Vector u = cross_product(G_CAM_UP, w);
	if(w.Length() < DBL_MIN
		|| u.Length() < DBL_MIN)
	{
		throw std::exception("wrong camera and center definition");
	}
	w.Normalize();
	u.Normalize();
	Vector v = cross_product(w, u);	// w和u正确，那么v一定也正确
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
	double tmax = DBL_MAX;	// 视点射出的光线是无穷远的
	double tan_fov_d2 = tan(xf::radians(G_FIELD_OF_VIEW / 2.0));
	double tan_fov_d2_dh = tan_fov_d2 / G_HEIGHT;
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
	// 返回值
	Vector color;
	for(int i = 0; i < G_HEIGHT; ++i)	// 对图片中的每行（从左下角开始）
	{
		// 输出渲染进度
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
		// 对行中的每个元素（从左开始）
		for(int j = 0; j < G_WIDTH; ++j)	
		{
			//cout << j << ' ';
			// 求出点(i,j)在相机坐标系的位置
			//Vector position_in_cam((j - G_WIDTH / 2.0 + 0.5) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0 + 0.5) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), -1.0);	// 原算式
			Vector position_in_cam((2.0 * j - G_WIDTH + 1.0) * tan_fov_d2_dh, (2.0 * i - G_HEIGHT + 1.0) * tan_fov_d2_dh, -1.0);	// 化简后的算式
			// 世界坐标系中的位置
			Vector position_in_global = mInverseTransform * position_in_cam;
			// 先计算出差值，判断长度后，再计算方向
			Vector direction = position_in_global - G_CAM_LOOKFROM;
			if(direction.Length() > DBL_MIN)	// 判断一下，距离不能太近 
			{
				// 变换为方向
				direction.Normalize();
				// 生成光线
				Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
				// 确定该点的颜色
				color = TraceRay(0, ray);
				if(color.x_ > 1.0)
				{
					color.x_ = 1.0;
				}
				if(color.y_ > 1.0)
				{
					color.y_ = 1.0;
				}
				if(color.z_ > 1.0)
				{
					color.z_ = 1.0;
				}
			}
			//else
			//{
				// 不给color赋值，表示color使用上一个颜色的值
				//color.x_ = color.y_ = color.z_ = 0.0;
			//}
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