#include "GlobalTerm.h"

// TODO 参数的合法性检验
void ContentParse(const string &scene_file, ostream &out)
{
	string line;
	ifstream f_in(scene_file);
	if(!f_in)
	{
		perror("scene_file open error: ");
		exit(EXIT_FAILURE);
	}

	istringstream buffer;
	string command;
	
	// 控制变量
	bool independent_mirror = false;
	// 持久变量（跨循环使用）
	Material material;
	vector<Vector> vertices;
	// 辅助变量（免除循环内定义的变量）
	Vector vec_general;
	int v1, v2, v3, verts_num;
	Triangle triangle;
	Sphere sphere;
	Light light;
	double degrees;
	// 计数变量
	int count = 0;
	cout << '\n';
	while(std::getline(f_in, line))
	{
		++count;
		cout << "\r\tline " << count << "...";
		//out << line << '\n';
		if(0 == line.size()
			|| '#' == line.at(0))
		{
			continue;
		}
		buffer.clear();		// 不知道为什么，一定要加这个，否则camera那行就会读不进去
		buffer.str(line);
		buffer >> command;
		if("size" == command)
		{
			buffer >> G_WIDTH >> G_HEIGHT;
		}
		else if("maxdepth" == command)
		{
			buffer >> G_MAXDEPTH;
		}
		else if("output" == command)
		{
			buffer >> G_OUTPUT_FILENAME;
		}
		else if("camera" == command)
		{
			buffer >> G_CAM_LOOKFROM.x_ >> G_CAM_LOOKFROM.y_ >> G_CAM_LOOKFROM.z_
				   >> G_CAM_LOOKAT.x_   >> G_CAM_LOOKAT.y_   >> G_CAM_LOOKAT.z_
				   >> G_CAM_UP.x_       >> G_CAM_UP.y_       >> G_CAM_UP.z_
				   >> G_FIELD_OF_VIEW;
		}
		else if("sphere" == command)
		{
			buffer >> sphere.center_.x_ >> sphere.center_.y_ >> sphere.center_.z_ >> sphere.radius_;
			sphere.material_ = material;
			sphere.inv_transform_mat_ = G_CUR_TRANSFORM_MATRIX.GetInverse();
			G_SPHERE_LIST.push_back(sphere);
		}
		else if("tri" == command)
		{
			buffer >> v1 >> v2 >> v3;
			triangle.va_ = vertices.at(v1);
			triangle.vb_ = vertices.at(v2);
			triangle.vc_ = vertices.at(v3);
			// 求法向量
			triangle.normal_ = xf::cross_product(vertices.at(v2) - vertices.at(v1), vertices.at(v3) - vertices.at(v2)).Normalize();
			triangle.material_ = material;
			G_TRIANGLE_LIST.push_back(Transform(G_CUR_TRANSFORM_MATRIX, triangle));
		}
		else if("vertex" == command)
		{
			buffer >> vec_general.x_ >> vec_general.y_ >> vec_general.z_;
			vertices.push_back(vec_general);
			//vertices.push_back(G_CUR_TRANSFORM_MATRIX * vec_general);
		}
		else if("translate" == command)
		{
			Vector translate_vec;
			buffer >> translate_vec.x_ >> translate_vec.y_ >> translate_vec.z_;
			Matrix translate_mat = TranslateMatrix(translate_vec);
#ifdef _DEBUG
			out << "translation" << '\n';
			out << translate_mat;
#endif
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * translate_mat;
		}
		else if("rotate" == command)
		{
			Vector rotation_vec;
			buffer >> rotation_vec.x_ >> rotation_vec.y_ >> rotation_vec.z_ >> degrees;
			Matrix rotation_mat = RotateMatrix(rotation_vec, degrees);
#ifdef _DEBUG
			out << "rotation" << '\n';
			out << rotation_mat;
#endif
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * rotation_mat;
		}
		else if("scale" == command)
		{
			Vector scale_vec;
			buffer >> scale_vec.x_ >> scale_vec.y_ >> scale_vec.z_;
			Matrix scale_mat = ScaleMatrix(scale_vec);
#ifdef _DEBUG
			out << "scale" << '\n';
			out << scale_mat;
#endif
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * scale_mat;
		}
		else if("pushTransform" == command)
		{
			G_MODELVIEWMATRIX_STACK.push(G_CUR_TRANSFORM_MATRIX);
#ifdef _DEBUG
			out << "after push, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << '\n';
#endif
		}
		else if("popTransform" == command)
		{
			G_CUR_TRANSFORM_MATRIX = G_MODELVIEWMATRIX_STACK.top();
			G_MODELVIEWMATRIX_STACK.pop();
#ifdef _DEBUG
			out << "after pop, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << '\n';
#endif
		}
		else if("point" == command)
		{
			buffer >> light.origin_.x_ >> light.origin_.y_ >> light.origin_.z_
				>> light.color_.x_ >> light.color_.y_ >> light.color_.z_;
			light.origin_ = G_CUR_TRANSFORM_MATRIX * light.origin_;
			G_POINTLIGHT_LIST.push_back(light);
		}
		else if("directional" == command)
		{
			buffer >> light.origin_.x_ >> light.origin_.y_ >> light.origin_.z_
				>> light.color_.x_ >> light.color_.y_ >> light.color_.z_;
			light.origin_ = G_CUR_TRANSFORM_MATRIX.TransformDirection(light.origin_);
			G_DIRECTIONALLIGHT_LIST.push_back(light);
		}
		else if("attenuation" == command)
		{
			buffer >> G_ATTENUATION.x_ >> G_ATTENUATION.y_ >> G_ATTENUATION.z_;
		}
		else if("ambient" == command)
		{
			buffer >> material.ambient_.r_ >> material.ambient_.g_ >> material.ambient_.b_;
		}
		else if("emission" == command)
		{
			buffer >> material.emission_.r_ >> material.emission_.g_ >> material.emission_.b_;
		}
		else if("diffuse" == command)
		{
			buffer >> material.diffuse_.r_ >> material.diffuse_.g_ >> material.diffuse_.b_;
		}
		else if("specular" == command)
		{
			buffer >> material.specular_.r_ >> material.specular_.g_ >> material.specular_.b_;
			if(!independent_mirror)
			{
				material.mirror_coefficient_ = material.specular_;
			}
		}
		else if("shininess" == command)
		{
			buffer >> material.shininess_;
		}
		else if("mirror" == command)
		{
			if(independent_mirror)
			{
				buffer >> material.mirror_coefficient_.r_ >> material.mirror_coefficient_.g_ >> material.mirror_coefficient_.b_;
			}
		}
		else if("maxverts" == command)	// 确定顶点总数
		{
			buffer >> verts_num;
			vertices.reserve(verts_num);
		}
		else if("IndependentMirrorOn" == command)
		{
			independent_mirror = true;
		}
		else if("IndependentMirrorOff" == command)
		{
			independent_mirror = false;
		}
		else if("DirectionalUniverseOn" == command)
		{
			G_DIRECTIONAL_UNIVERSE = true;
		}
		else if("DirectionalUniverseOff" == command)
		{
			G_DIRECTIONAL_UNIVERSE = false;
		}
		else if("maxvertnorms" == command)
		{
			assert(false);
		}
		else
		{
			cout << "havn't handle this: " << line << '\n';
			assert(false);
		}
	}

#ifdef _DEBUG
	cout << "\n\tprint parameter...";
	// GLOBAL TERM
	out << "widht = " << G_WIDTH << "\theight = " << G_HEIGHT << '\n';
	out << "max depth = " << G_MAXDEPTH << '\n';
	out << "result file name:     " << G_OUTPUT_FILENAME << '\n';
	out << "camera look from:     " << G_CAM_LOOKFROM.x_ << ", " << G_CAM_LOOKFROM.y_ << ", " << G_CAM_LOOKFROM.z_ << '\n'
		 << "camera look at:       " << G_CAM_LOOKAT.x_   << ", " << G_CAM_LOOKAT.y_   << ", " << G_CAM_LOOKAT.z_ << '\n'
		 << "camera up direction:  " << G_CAM_UP.x_       << ", " << G_CAM_UP.y_       << ", " << G_CAM_UP.z_ << '\n'
		 << "camera field of view: " << G_FIELD_OF_VIEW << '\n';
	// LIGHTING
	cout << "\n\tprint lighting...";
	out << "point light: " << '\n';
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		out << "\tposition: (" << c_iter->origin_.x_ << ", " << c_iter->origin_.y_ << ", " << c_iter->origin_.z_ << ") "
			 << "\tcolor: (" << c_iter->color_.x_ << ", " << c_iter->color_.y_ << ", " << c_iter->color_.z_ << ")" << '\n';
	}
	out << "directional light: " << '\n';
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
		c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		out << "\tdirection: (" << c_iter->origin_.x_ << ", " << c_iter->origin_.y_ << ", " << c_iter->origin_.z_ << ") "
			 << "\tcolor: (" << c_iter->color_.x_ << ", " << c_iter->color_.y_ << ", " << c_iter->color_.z_ << ")" << '\n';
	}
	out << "Attenuation: " << G_ATTENUATION.x_ << ", " << G_ATTENUATION.y_ << ", " << G_ATTENUATION.z_ << '\n';
	// SPHERE
	cout << "\n\tprint sphere...";
	out << "Sphere: " << '\n';
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		out << distance(G_SPHERE_LIST.cbegin(), c_iter) << ":" << '\t'
             << "center: (" << c_iter->center_.x_ << ", " << c_iter->center_.y_ << ", " << c_iter->center_.z_ << "), original radius: " << c_iter->radius_ << '\n'  
			 << "ambient: " << c_iter->material_.ambient_.r_ << ", " << c_iter->material_.ambient_.g_ << ", " << c_iter->material_.ambient_.b_ << '\n'
			 << "material:"   << '\n'
			 << "\tdiffuse_: "   << c_iter->material_.diffuse_.r_  << ", " << c_iter->material_.diffuse_.g_  << ", " << c_iter->material_.diffuse_.b_  << '\n'
			 << "\tspecular_: "  << c_iter->material_.specular_.r_ << ", " << c_iter->material_.specular_.g_ << ", " << c_iter->material_.specular_.b_ << '\n'
			 << "\tshininess_: " << c_iter->material_.shininess_   << '\n'
			 << "\temission_: "  << c_iter->material_.emission_.r_ << ", " << c_iter->material_.emission_.g_ << ", " << c_iter->material_.emission_.b_ << '\n';
	}
	// TRIANGLE
	cout << "\n\tprint triangle...";
	out << "Triangle: " << '\n';
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		out << distance(G_TRIANGLE_LIST.cbegin(), c_iter) << ":" << '\t'
			 << "(" << c_iter->va_.x_ << ", " << c_iter->va_.y_ << ", " << c_iter->va_.z_ << "), "
			 << "(" << c_iter->vb_.x_ << ", " << c_iter->vb_.y_ << ", " << c_iter->vb_.z_ << "), "
			 << "(" << c_iter->vc_.x_ << ", " << c_iter->vc_.y_ << ", " << c_iter->vc_.z_ << "), "<< '\n'
			 << "ambient: " << c_iter->material_.ambient_.r_ << ", " << c_iter->material_.ambient_.g_ << ", " << c_iter->material_.ambient_.b_ << '\n'
			 << "material:"   << '\n'
			 << "\tdiffuse_: "   << c_iter->material_.diffuse_.r_  << ", " << c_iter->material_.diffuse_.g_  << ", " << c_iter->material_.diffuse_.b_  << '\n'
			 << "\tspecular_: "  << c_iter->material_.specular_.r_ << ", " << c_iter->material_.specular_.g_ << ", " << c_iter->material_.specular_.b_ << '\n'
			 << "\tshininess_: " << c_iter->material_.shininess_   << '\n'
			 << "\temission_: "  << c_iter->material_.emission_.r_ << ", " << c_iter->material_.emission_.g_ << ", " << c_iter->material_.emission_.b_ << '\n';
	}
#endif
}
