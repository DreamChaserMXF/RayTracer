#include "GlobalTerm.h"

BYTE* RayTracer()
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
	cout << mTransform << endl;
	//Matrix mInverseTransform = mTransform.Inverse();
	// һ���µ�����ķ���
	mTranslate[0][3] = -mTranslate[0][3];
	mTranslate[1][3] = -mTranslate[1][3];
	mTranslate[2][3] = -mTranslate[2][3];
	Matrix mInverseTransform = mTranslate * mRotate.Tranpose();
	cout << mInverseTransform << endl;
	// Ԥ�ȼ�����
	double tan_fov_d2 = tan(xf::radians(G_FIELD_OF_VIEW / 2.0));
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// ��TeapotViewing���ַ�ʽȥ���������в�ͨ�ģ���֪��Ϊʲô
	for(int i = 0; i < G_HEIGHT; ++i)
	{
		for(int j = 0; j < G_WIDTH; ++j)
		{
			// �����(i,j)���������ϵ��λ��
			//Vector position_in_cam((j - G_WIDTH / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0) * tan(xf::radians(G_FIELD_OF_VIEW / 2.0)) / (G_HEIGHT / 2.0), -1.0);	// ԭ��ʽ
			Vector position_in_cam((2.0 * j - G_WIDTH) * tan_fov_d2 / G_HEIGHT, (2.0 * i / G_HEIGHT - 1.0) * tan_fov_d2, -1.0);	// ��������ʽ
			// ��������ϵ�е�λ��
			Vector position_in_global = mInverseTransform * position_in_cam;
			// ����
			Vector direction = (position_in_global - G_CAM_LOOKFROM).Normalize();
			double tmin = (position_in_global - G_CAM_LOOKFROM).Length();
			double tmax = std::numeric_limits<double>::max();
			// ���ɹ���
			Ray ray(G_CAM_LOOKFROM, direction, tmin, tmax);
			// ȷ���õ����ɫ
			Vector color = TraceColor(ray);
			// �洢ͼƬ��ɫʱ����Ҫ��BGR�ĸ�ʽ
			pixels[(i * G_WIDTH + j) * 3 + 0] = static_cast<BYTE>(color.b_);
			pixels[(i * G_WIDTH + j) * 3 + 1] = static_cast<BYTE>(color.g_);
			pixels[(i * G_WIDTH + j) * 3 + 2] = static_cast<BYTE>(color.r_);
		}
	}
	return pixels;
}

//
//Vector TraceColor(const Ray &ray)
//{
//	const Vector kBackGroundColor;
//	//const Vector kBackGroundColor(128, 128, 255);
//	enum {NONE, SPHERE, TRIANGLE} intersection_type = NONE;
//	// ��������
//	double min_distance = std::numeric_limits<double>::max();
//	// ������
//	Vector tmp1, tmp2, scale;
//	list<Sphere>::const_iterator sphere_iter;
//	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
//		c_iter != G_SPHERE_LIST.end(); ++c_iter)
//	{
//		Vector tmp(c_iter->transform_mat_[0][0], c_iter->transform_mat_[1][1], c_iter->transform_mat_[2][2]);
//		scale.x_ = c_iter->transform_mat_[0][0] + c_iter->transform_mat_[0][1] + c_iter->transform_mat_[0][2];
//		scale.y_ = c_iter->transform_mat_[1][0] + c_iter->transform_mat_[1][1] + c_iter->transform_mat_[1][2];
//		scale.z_ = c_iter->transform_mat_[2][0] + c_iter->transform_mat_[2][1] + c_iter->transform_mat_[2][2];
//
//		tmp1 = (ray.direction_ / scale);
//		tmp2 = (ray.origin_ - c_iter->center_) / scale;
//		double a = tmp1 * tmp1;
//		double b = 2 * tmp1 * tmp2;
//		double c = tmp2 * tmp2 - c_iter->radius_ * c_iter->radius_;
//		double delta = b * b - 4.0 * a * c;
//		if(abs(delta) < xf::EPS)	// ֱ������ֻ��һ������
//		{
//			// ���㽻��Ĳ���t
//			double t = -b / 2.0 / a;
//			if(t >= ray.tmin_ && t < min_distance)	// �߶������ཻ���ҽ������
//			{
//				min_distance = t;
//				intersection_type = SPHERE;
//				// TODO ���������ĵ�����
//				sphere_iter = c_iter;
//			}
//		}
//		else if(delta > 0.0)	// ���������㣬���������t��
//		{
//			// t1 < t2
//			double t1 = (-b - sqrt(delta)) / 2.0 / a;
//			double t2 = (-b + sqrt(delta)) / 2.0 / a;
//			// ���t1����Ҫ��
//			if(t1 >= ray.tmin_ && t1 < min_distance)
//			{
//				min_distance = t1;
//				intersection_type = SPHERE;
//				// TODO ���������ĵ�����
//				sphere_iter = c_iter;
//			}
//			else if(t2 >= ray.tmin_ && t2 < min_distance)
//			{
//				min_distance = t1;
//				intersection_type = SPHERE;
//				// TODO ���������ĵ�����
//				sphere_iter = c_iter;
//			}
//		}
//	}
//
//	// ����������
//	list<Triangle>::const_iterator triangle_iter;
//	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
//		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
//	{
//		// ���ж�ֱ�ߵķ������������Ƿ���
//		if(abs(cos(ray.direction_, c_iter->normal_)) > std::numeric_limits<double>::min())
//		{
//			// ����������
//			// ��ֱ��һ�������������ڵ�ƽ���ཻ
//			// �󽻵�Ĳ���t
//			double t = ((c_iter->v1_ - ray.origin_) * c_iter->normal_) / (ray.direction_ * c_iter->normal_);
//			if(t >= ray.tmin_ && t < min_distance)
//			{
//				// �󽻵�
//				Vector intersection_point = ray.origin_ + t * ray.direction_;
//				// �жϸõ��Ƿ�����������
//				// �нǶȷ����������ͬ�򷨡����ķ����㷨��������������
//				// �ȼ��������ε����
//				double tri_aera = cross(c_iter->v2_ - c_iter->v1_, c_iter->v3_ - c_iter->v1_).Length() / 2.0;
//				// �ټ�����intersection_pointΪ���������С�����ε����
//				double aera1 = cross(intersection_point - c_iter->v1_, intersection_point - c_iter->v2_).Length() / 2.0;
//				double aera2 = cross(intersection_point - c_iter->v2_, intersection_point - c_iter->v3_).Length() / 2.0;
//				double aera3 = cross(intersection_point - c_iter->v3_, intersection_point - c_iter->v1_).Length() / 2.0;
//				// �ȽϿ�����С�����ε�������Ƿ���ڴ������ε����
//				if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// ��ȣ�˵��������
//				{
//					min_distance = t;
//					intersection_type = TRIANGLE;
//					// ��¼ָ��������εĵ�����
//					triangle_iter = c_iter;
//				}
//			}
//		}
//	}
//	// ȷ����ɫ
//	Vector color;
//	// ȷ���������
//	if(NONE == intersection_type)
//	{
//		color = kBackGroundColor;
//	}
//	else if(TRIANGLE == intersection_type)
//	{
//		// ����˴�����ɫ
//		color = triangle_iter->ambient_ + triangle_iter->material_.emission_;
//		color *= 255;
//	}
//	else
//	{
//		assert(SPHERE == intersection_type);
//		color = sphere_iter->ambient_ + sphere_iter->material_.emission_;
//		color *= 255;
//	}
//	return color;
//}




Vector TraceColor(const Ray &ray)
{
	const Vector kBackGroundColor;
	//const Vector kBackGroundColor(128, 128, 255);
	enum {NONE, SPHERE, TRIANGLE} intersection_type = NONE;
	// ��������
	double min_distance = std::numeric_limits<double>::max();
	// ������
	Vector tmp1, tmp2, scale;
	list<Sphere>::const_iterator sphere_iter;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		// ���ӵ�ͷ���任��ȥ
		Vector origin = c_iter->transform_mat_.Inverse() * ray.origin_;
		Vector direction = c_iter->transform_mat_.Inverse().TransformDirection(ray.direction_);

		double a = direction * direction;
		double b = 2.0 * direction * (origin - c_iter->center_);
		double c = (origin - c_iter->center_) * (origin - c_iter->center_) - c_iter->radius_ * c_iter->radius_;
		double delta = b * b - 4.0 * a * c;
		if(abs(delta) < xf::EPS)	// ֱ������ֻ��һ������
		{
			// ���㽻��Ĳ���t
			double t = -b / 2.0 / a;
			if(t >= ray.tmin_ && t < min_distance)	// �߶������ཻ���ҽ������
			{
				min_distance = t;
				intersection_type = SPHERE;
				// TODO ���������ĵ�����
				sphere_iter = c_iter;
			}
		}
		else if(delta > 0.0)	// ���������㣬���������t��
		{
			// t1 < t2
			double t1 = (-b - sqrt(delta)) / 2.0 / a;
			double t2 = (-b + sqrt(delta)) / 2.0 / a;
			// ���t1����Ҫ��
			if(t1 >= ray.tmin_ && t1 < min_distance)
			{
				min_distance = t1;
				intersection_type = SPHERE;
				// TODO ���������ĵ�����
				sphere_iter = c_iter;
			}
			else if(t2 >= ray.tmin_ && t2 < min_distance)
			{
				min_distance = t1;
				intersection_type = SPHERE;
				// TODO ���������ĵ�����
				sphere_iter = c_iter;
			}
		}
	}

	// ����������
	list<Triangle>::const_iterator triangle_iter;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		// ���ж�ֱ�ߵķ������������Ƿ���
		if(abs(cos(ray.direction_, c_iter->normal_)) > std::numeric_limits<double>::min())
		{
			// ����������
			// ��ֱ��һ�������������ڵ�ƽ���ཻ
			// �󽻵�Ĳ���t
			double t = ((c_iter->v1_ - ray.origin_) * c_iter->normal_) / (ray.direction_ * c_iter->normal_);
			if(t >= ray.tmin_ && t < min_distance)
			{
				// �󽻵�
				Vector intersection_point = ray.origin_ + t * ray.direction_;
				// �жϸõ��Ƿ�����������
				// �нǶȷ����������ͬ�򷨡����ķ����㷨��������������
				// �ȼ��������ε����
				double tri_aera = cross(c_iter->v2_ - c_iter->v1_, c_iter->v3_ - c_iter->v1_).Length() / 2.0;
				// �ټ�����intersection_pointΪ���������С�����ε����
				double aera1 = cross(intersection_point - c_iter->v1_, intersection_point - c_iter->v2_).Length() / 2.0;
				double aera2 = cross(intersection_point - c_iter->v2_, intersection_point - c_iter->v3_).Length() / 2.0;
				double aera3 = cross(intersection_point - c_iter->v3_, intersection_point - c_iter->v1_).Length() / 2.0;
				// �ȽϿ�����С�����ε�������Ƿ���ڴ������ε����
				if(abs(aera1 + aera2 + aera3 - tri_aera) < xf::EPS)	// ��ȣ�˵��������
				{
					min_distance = t;
					intersection_type = TRIANGLE;
					// ��¼ָ��������εĵ�����
					triangle_iter = c_iter;
				}
			}
		}
	}
	// ȷ����ɫ
	Vector color;
	// ȷ���������
	if(NONE == intersection_type)
	{
		color = kBackGroundColor;
	}
	else if(TRIANGLE == intersection_type)
	{
		// ����˴�����ɫ
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