#include "GlobalTerm.h"
#include <ctime>



static bool IsVisible(const Ray &ray, Vector &refraction_coef)
{
	double t = 0.0;
	//refraction_coef.x_ = refraction_coef.y_ = refraction_coef.z_ = 1.0;
	// ������
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			if(c_iter->material_.refraction_coefficient_.Length() > DBL_MIN)
			{
				// ����Ľ����˵�ϵ��ҪСһЩ����Ϊ�ύ����
				refraction_coef = scale_product(refraction_coef, c_iter->material_.refraction_coefficient_ + (Vector(1.0, 1.0, 1.0) - c_iter->material_.refraction_coefficient_) / 2.0);
			}
			else
			{
				return false;
			}
		}
	}
	// ����������
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
	// ��ʼ������
	Vector color;
	// �ӵ��붥���غ�
	Vector view_vec = eye - vertex;
	if(view_vec.Length() < DBL_MIN)
	{
		return color;
	}
	view_vec.Normalize();

	// ���Դ
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		// �ȼ�����Դ�Ըõ��Ƿ�ɼ�
		Vector diff_vec = c_iter->origin_ - vertex;
		// ��������Դ�غ�
		double distance = diff_vec.Length();
		if(distance > DBL_MIN)	// ��Դ�뽻��֮��һ��Ҫ�о������
		{
			Vector light_direction = diff_vec.GetNormalize();	// �о���ʱ���Ϳ��Թ�һ��
			Vector half_vec = view_vec + light_direction;
			if(half_vec.Length() > DBL_MIN)	// ���������߲��ܹ��߷���
			{
				half_vec.Normalize();
				Ray light_ray(vertex, light_direction, xf::EPS, distance);	// imp�� EPS������0�����tminΪ0�Ļ�����ô����ʼ���һ������õ��ý�����Ǹ��������ཻ
				Vector refraction_coef(1.0, 1.0, 1.0);
				if(IsVisible(light_ray, refraction_coef))	// �ɼ�
				{
					Vector point_color = scale_product(c_iter->color_ / (G_ATTENUATION.x_ + G_ATTENUATION.y_ * distance + G_ATTENUATION.z_ * distance * distance),	// ˥��ϵ��
								(material.diffuse_ 
								* xf::max(normal * light_direction, 0.0))		                               // �����
							+ (material.specular_ 
								* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // �����
						);	
					color += scale_product(refraction_coef, point_color);	// refraction_coef���м����������ϵ��
				}
			}
		}
	}
	// �����
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
	c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		Vector half_vec = view_vec + c_iter->origin_;
		if(half_vec.Length() > DBL_MIN)	// ���������߲��ܹ��߷���
		{
			half_vec.Normalize();
			Ray light_ray(vertex, c_iter->origin_, xf::EPS, DBL_MAX);	// imp�� EPS������0�����tminΪ0�Ļ�����ô����ʼ���һ������õ��ý�����Ǹ��������ཻ
			Vector refraction_coef(1.0, 1.0, 1.0);
			if(G_DIRECTIONAL_UNIVERSE || IsVisible(light_ray, refraction_coef))	// �ɼ�
			{
				Vector directional_color = scale_product(c_iter->color_,
							(material.diffuse_ 
							* xf::max(normal * c_iter->origin_, 0.0))	// �����
						+ (material.specular_ 
							* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // �����
					);	
				color += scale_product(refraction_coef, directional_color);	// refraction_coef���м����������ϵ��
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
	// ��
	Primitive intersection_type = NONE;
	double min_distance = ray.tmax_;
	double t = 0.0;
	// ������
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
	// ����������
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
	// ȷ����ɫ
	Vector color;
	// ȷ���������
	if(TRIANGLE == intersection_type)
	{
		// ������+�Է���
		color = triangle_iter->material_.ambient_ + triangle_iter->material_.emission_;
		// ���㽻��λ��
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// ����ֱ�ӹ���
		color += ComputeLight(depth, ray.origin_, intersection_point, triangle_iter->normal_, triangle_iter->material_);
		// ���㷴���
		Vector reflection_direction = ray.direction_ - 2.0 * triangle_iter->normal_ * ray.direction_ * triangle_iter->normal_;
		if(triangle_iter->material_.mirror_coefficient_.Length() > DBL_MIN)
		{
			Ray reflection(intersection_point, reflection_direction, xf::EPS, DBL_MAX);
			color += scale_product(triangle_iter->material_.mirror_coefficient_, TraceRay(depth + 1, reflection));
		}
		// ��ɫ��
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
		// ������+�Է���
		color = sphere_iter->material_.ambient_ + sphere_iter->material_.emission_;
		// ���㽻��λ��
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// ���㽻�㷨��
		// 1. �ѽ���任��ԭλ�� 2. ���ԭλ�õķ��� 3. �ѷ���任�����շ���
		Vector original_point = sphere_iter->inv_transform_mat_ * intersection_point;
		Vector original_normal = (original_point - sphere_iter->center_).Normalize();	// parse��ʱ������ˣ���İ뾶������Ϊ0����һ���������쳣
		Vector normal = sphere_iter->inv_transform_mat_.GetTranspose().TransformDirection(original_normal).Normalize();	// ����任֮��Ҳһ������Ϊ0����
		// ����ֱ�ӹ���
		color += ComputeLight(depth, ray.origin_, intersection_point, normal, sphere_iter->material_);
		// ���������
		Vector mirror_gain;	// ����ǹ��������ȫ���������µķ���ϵ������
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
		// ���㷴���
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

		// ��ɫ��
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
	// ������������ϵ����������ı�ʾ����mRotate
	Vector w = (G_CAM_LOOKFROM - G_CAM_LOOKAT);
	Vector u = cross_product(G_CAM_UP, w);
	if(w.Length() < DBL_MIN
		|| u.Length() < DBL_MIN)
	{
		throw std::exception("wrong camera and center definition");
	}
	w.Normalize();
	u.Normalize();
	Vector v = cross_product(w, u);	// w��u��ȷ����ôvһ��Ҳ��ȷ
	Matrix mRotate(4);
	// ���������ǧ���㷴��
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
	// ����ƽ�ƾ���mTranslate
	Matrix mTranslate = TranslateMatrix(-G_CAM_LOOKFROM);
	// ������������ϵ���������ϵ��ת������mTransform
	// ��mTransform���棬�����������ϵ����ת��Ϊ��������ϵ������ת������mCam2World
	Matrix mTransform = mRotate * mTranslate;
	//cout << mTransform << endl;
	//Matrix mInverseTransform = mTransform.GetInverse();
	// һ���µ�����ķ���
	mTranslate[0][3] = -mTranslate[0][3];
	mTranslate[1][3] = -mTranslate[1][3];
	mTranslate[2][3] = -mTranslate[2][3];
	Matrix mInverseTransform = mTranslate * mRotate.GetTranspose();
	//cout << mInverseTransform << endl;
	// Ԥ�ȼ�����
	double tmin = 0;
	double tmax = DBL_MAX;	// �ӵ�����Ĺ���������Զ��
	double tan_fov_d2 = tan(xf::radians(G_FIELD_OF_VIEW / 2.0));
	double tan_fov_d2_dh = tan_fov_d2 / G_HEIGHT;
	// �洢���ص�����
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// ��TeapotViewing���ַ�ʽȥ���������в�ͨ�ģ���֪��Ϊʲô
	// ���ڼ�����ȵı���
	int one_percent = G_HEIGHT / 100;
	int percentage = 0;
	time_t current_t = time(NULL);
	time_t last_t = current_t;
	int dot_index = 0;
	const char* dotclear_str[] = {"\b \b", "\b\b  \b\b", "\b\b\b   \b\b\b"};
	const char* dot_str[] = {".", "..", "..."};
	cout << "0%.";
	// ����ֵ
	Vector color;
	for(int i = 0; i < G_HEIGHT; ++i)	// ��ͼƬ�е�ÿ�У������½ǿ�ʼ��
	{
		// �����Ⱦ����
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
		// �����е�ÿ��Ԫ�أ�����ʼ��
		for(int j = 0; j < G_WIDTH; ++j)	
		{
			//cout << j << ' ';
			// �����(i,j)���������ϵ��λ��
			//Vector position_in_cam((j - G_WIDTH / 2.0 + 0.5) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0 + 0.5) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), -1.0);	// ԭ��ʽ
			Vector position_in_cam((2.0 * j - G_WIDTH + 1.0) * tan_fov_d2_dh, (2.0 * i - G_HEIGHT + 1.0) * tan_fov_d2_dh, -1.0);	// ��������ʽ
			// ��������ϵ�е�λ��
			Vector position_in_global = mInverseTransform * position_in_cam;
			// �ȼ������ֵ���жϳ��Ⱥ��ټ��㷽��
			Vector direction = position_in_global - G_CAM_LOOKFROM;
			if(direction.Length() > DBL_MIN)	// �ж�һ�£����벻��̫�� 
			{
				// �任Ϊ����
				direction.Normalize();
				// ���ɹ���
				Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
				// ȷ���õ����ɫ
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
				// ����color��ֵ����ʾcolorʹ����һ����ɫ��ֵ
				//color.x_ = color.y_ = color.z_ = 0.0;
			//}
			// �洢ͼƬ��ɫʱ����Ҫ��BGR�ĸ�ʽ
			// ����Ҫ��˫���ȵ���ɫת��ΪBYTE��ʽ����ɫ
			pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(color.b_ * 255.0);
			pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(color.g_ * 255.0);
			pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(color.r_ * 255.0);
		}
	}
	cout << "\r                            \r\t100%\n";
	return pixels;
}