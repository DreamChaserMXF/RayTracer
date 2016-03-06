#include "GlobalTerm.h"
#include <ctime>



static bool IsVisible(const Ray &ray)
{
	double t = 0.0;
	// ������
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		if(c_iter->IntersectTest(ray, t))
		{
			return false;
		}
	}
	// ����������
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
	// ���Դ
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		// �ȼ�����Դ�Ըõ��Ƿ�ɼ�
		Vector diff_vec = c_iter->origin_ - vertex;
		Vector light_direction = diff_vec.GetNormalize();	// imp! GetXXX
		double distance = diff_vec.Length();
		if(distance > DBL_MIN)	// ��Դ�뽻��֮��һ��Ҫ�о������
		{
			Vector half_vec = ((eye - vertex).Normalize() + light_direction).Normalize();
			if(half_vec.Length() > DBL_MIN)	// ���������߲��ܹ��߷���
			{
				Ray light_ray(vertex, light_direction, xf::EPS * 10, distance);	// imp�� EPS������0�����tminΪ0�Ļ�����ô����ʼ���һ������õ��ý�����Ǹ��������ཻ
				if(IsVisible(light_ray))	// �ɼ�
				{
					Vector point_color = dot_cross(c_iter->color_ / (G_ATTENUATION.x_ + G_ATTENUATION.y_ * distance + G_ATTENUATION.z_ * distance * distance),	// ˥��ϵ��
								(material.diffuse_ 
								* xf::max(normal * light_direction, 0.0))		                               // �����
							+ (material.specular_ 
								* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // �����
						);	
					color += point_color;
				}
			}
		}
	}
	// �����
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
	c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		// �ȼ�����Դ�Ըõ��Ƿ�ɼ�
		Vector light_direction = (c_iter->origin_).GetNormalize();
		Vector half_vec = ((eye - vertex).Normalize() + light_direction).Normalize();
		if(half_vec.Length() > DBL_MIN)	// ���������߲��ܹ��߷���
		{
			Ray light_ray(vertex, light_direction, xf::EPS * 10, DBL_MAX);	// imp�� EPS������0�����tminΪ0�Ļ�����ô����ʼ���һ������õ��ý�����Ǹ��������ཻ
			if(IsVisible(light_ray))	// �ɼ�
			{
				Vector directional_color = dot_cross(c_iter->color_,	// ˥��ϵ��
							(material.diffuse_ 
							* xf::max(normal * light_direction, 0.0))	// �����
						+ (material.specular_ 
							* pow(xf::max(normal * half_vec, 0.0), material.shininess_)) // �����
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
		color = triangle_iter->ambient_ + triangle_iter->material_.emission_;
		// ���㽻��λ��
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// ����������뾵���
		color += ComputeLight(ray.origin_, intersection_point, triangle_iter->normal_, triangle_iter->material_);
		// ���㷴���
		if(triangle_iter->material_.specular_.Length() > DBL_MIN)
		{
			Ray reflection(intersection_point, ray.direction_ - 2.0 * triangle_iter->normal_ * ray.direction_ * triangle_iter->normal_, xf::EPS * 10, DBL_MAX);
			color += dot_cross(triangle_iter->material_.specular_, TraceRay(depth + 1, reflection));
		}
	}
	else if(SPHERE == intersection_type)
	{
		// ������+�Է���
		color = sphere_iter->ambient_ + sphere_iter->material_.emission_;
		// ���㽻��λ��
		Vector intersection_point = ray.origin_ + ray.direction_ * min_distance;
		// ���㽻�㷨��
		// 1. �ѽ���任��ԭλ�� 2. ���ԭλ�õķ��� 3. �ѷ���任�����շ���
		Vector original_point = sphere_iter->inv_transform_mat_ * intersection_point;
		Vector original_normal = (original_point - sphere_iter->center_).Normalize();
		Vector normal = sphere_iter->inv_transform_mat_.GetTranspose().TransformDirection(original_normal).Normalize();
		// ����������뾵���
		color += ComputeLight(ray.origin_, intersection_point, normal, sphere_iter->material_);
		// ���㷴���
		if(sphere_iter->material_.specular_.Length() > DBL_MIN)
		{
			Ray reflection(intersection_point, ray.direction_ - 2.0 * normal * ray.direction_ * normal, xf::EPS * 10, DBL_MAX);
			color += dot_cross(sphere_iter->material_.specular_, TraceRay(depth + 1, reflection));
		}
	}
	return color;
}

BYTE* Render()
{
	// ������������ϵ����������ı�ʾ����mRotate
	Vector w = (G_CAM_LOOKFROM - G_CAM_LOOKAT).Normalize();
	Vector u = cross(G_CAM_UP, w).Normalize();
	Vector v = cross(w, u);
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
	// �洢���ص�����
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// ��TeapotViewing���ַ�ʽȥ����������в�ͨ�ģ���֪��Ϊʲô
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

		for(int j = 0; j < G_WIDTH; ++j)	// �����е�ÿ��Ԫ�أ�����ʼ��
		{
			// �����(i,j)���������ϵ��λ��
			//Vector position_in_cam((j - G_WIDTH / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), -1.0);	// ԭ��ʽ
			Vector position_in_cam((2.0 * j - G_WIDTH) * tan_fov_d2 / G_HEIGHT, (2.0 * i / G_HEIGHT - 1.0) * tan_fov_d2, -1.0);	// ��������ʽ
			// ��������ϵ�е�λ��
			Vector position_in_global = mInverseTransform * position_in_cam;
			// ����
			Vector direction = (position_in_global - G_CAM_LOOKFROM).Normalize();
			if(direction.Length() > DBL_MIN)	// �ж�һ�£����벻��̫�� 
			{
				// ���ɹ���
				Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
				// ȷ���õ����ɫ
				color = TraceRay(0, ray);
			}
			else
			{
				// ����color��ֵ����ʾcolorʹ����һ����ɫ��ֵ
				//color.x_ = color.y_ = color.z_ = 0.0;
			}
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