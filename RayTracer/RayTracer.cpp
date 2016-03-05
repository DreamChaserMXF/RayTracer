#include "GlobalTerm.h"

BYTE* RayTracer()
{
	// ������������ϵ����������ı�ʾ����mRotate
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
	// ��Ļ������ľ���
	double distance = 1.0 / tan(xf::radians(G_FIELD_OF_VIEW / 2));
	BYTE *pixels = new BYTE[G_HEIGHT * G_WIDTH * 3];	// ��TeapotViewing���ַ�ʽȥ���������в�ͨ�ģ���֪��Ϊʲô
	for(int i = 0; i < G_HEIGHT; ++i)
	{
		for(int j = 0; j < G_WIDTH; ++j)
		{
			// �����(i,j)���������ϵ��λ��
			Vector position_in_cam((G_WIDTH / 2.0 - j) / (G_HEIGHT / 2.0), (i - G_HEIGHT / 2.0) / (G_HEIGHT / 2.0), distance);
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
			// �洢��ɫ
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
			// �洢ͼƬʱ����BGR�ĸ�ʽ
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
	// ��������
	double min_distance = std::numeric_limits<double>::max();
	// ������
	list<Sphere>::const_iterator sphere_iter;
	double a = ray.direction_ * ray.direction_;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		double b = 2 * ray.direction_ * (ray.direction_ - c_iter->center_);
		double c = (ray.origin_ - c_iter->center_) * (ray.origin_ - c_iter->center_);
		double delta = b * b - 4 * a * c;
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