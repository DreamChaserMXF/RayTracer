#include "GlobalTerm.h"

void ContentParse(const string &scene_file)
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
	
	Material material;
	Vector ambient;
	
	Sphere sphere;
	vector<Vector> vertices;
	Triangle triangle;
	Light point_light;
	Light directional_light;
	
	
	
	while(std::getline(f_in, line))
	{
		//cout << line << endl;
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
			sphere.ambient_ = ambient;
			sphere.material_ = material;
			G_SPHERE_LIST.push_back(Transform(G_CUR_TRANSFORM_MATRIX, sphere));
		}
		else if("tri" == command)
		{
			int v1, v2, v3;
			buffer >> v1 >> v2 >> v3;
			triangle.v1_ = vertices.at(v1);
			triangle.v2_ = vertices.at(v2);
			triangle.v3_ = vertices.at(v3);
			// 求出法线
			triangle.normal_ = xf::cross(vertices.at(v2) - vertices.at(v1), vertices.at(v3) - vertices.at(v1)).Normalize();
			triangle.ambient_ = ambient;
			triangle.material_ = material;
			G_TRIANGLE_LIST.push_back(Transform(G_CUR_TRANSFORM_MATRIX, triangle));
		}
		else if("vertex" == command)
		{
			Vector vertex;
			buffer >> vertex.x_ >> vertex.y_ >> vertex.z_;
			vertices.push_back(vertex);
		}
		else if("translate" == command)
		{
			Vector translate_vec;
			buffer >> translate_vec.x_ >> translate_vec.y_ >> translate_vec.z_;
			Matrix translate_mat = TranslateMatrix(translate_vec);
			cout << "translation" << endl;
			cout << translate_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * translate_mat;
		}
		else if("rotate" == command)
		{
			Vector rotation_vec;
			double degrees;
			buffer >> rotation_vec.x_ >> rotation_vec.y_ >> rotation_vec.z_ >> degrees;
			Matrix rotation_mat = RotateMatrix(rotation_vec, degrees);
			cout << "rotation" << endl;
			cout << rotation_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * rotation_mat;
		}
		else if("scale" == command)
		{
			Vector scale_vec;
			buffer >> scale_vec.x_ >> scale_vec.y_ >> scale_vec.z_;
			Matrix scale_mat = ScaleMatrix(scale_vec);
			cout << "scale" << endl;
			cout << scale_mat;
			G_CUR_TRANSFORM_MATRIX = G_CUR_TRANSFORM_MATRIX * scale_mat;
		}
		else if("pushTransform" == command)
		{
			G_MODELVIEWMATRIX_STACK.push(G_CUR_TRANSFORM_MATRIX);
			cout << "after push, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << endl;
		}
		else if("popTransform" == command)
		{
			G_CUR_TRANSFORM_MATRIX = G_MODELVIEWMATRIX_STACK.top();
			G_MODELVIEWMATRIX_STACK.pop();
			cout << "after pop, size of matrix stack = " << G_MODELVIEWMATRIX_STACK.size() << endl;
		}
		else if("directional" == command)
		{
			buffer >> directional_light.origin_.x_ >> directional_light.origin_.y_ >> directional_light.origin_.z_
				>> directional_light.color_.x_ >> directional_light.color_.y_ >> directional_light.color_.z_;
			G_DIRECTIONALLIGHT_LIST.push_back(directional_light);
		}
		else if("point" == command)
		{
			buffer >> point_light.origin_.x_ >> point_light.origin_.y_ >> point_light.origin_.z_
				>> point_light.color_.x_ >> point_light.color_.y_ >> point_light.color_.z_;
			G_POINTLIGHT_LIST.push_back(point_light);
		}
		else if("attenuation" == command)
		{
			buffer >> G_ATTENUATION.x_ >> G_ATTENUATION.y_ >> G_ATTENUATION.z_;
		}
		else if("ambient" == command)
		{
			buffer >> ambient.r_ >> ambient.g_ >> ambient.b_;
		}
		else if("diffuse" == command)
		{
			buffer >> material.diffuse_.r_ >> material.diffuse_.g_ >> material.diffuse_.b_;
		}
		else if("specular" == command)
		{
			buffer >> material.specular_.r_ >> material.specular_.g_ >> material.specular_.b_;
		}
		else if("shininess" == command)
		{
			buffer >> material.shininess_;
		}
		else if("emission" == command)
		{
			buffer >> material.emission_.r_ >> material.emission_.g_ >> material.emission_.b_;
		}
		else if("maxverts" == command)	// 确定顶点总数
		{
			int verts_num;
			buffer >> verts_num;
			vertices.reserve(verts_num);
		}
		else if("maxvertnorms" == command)
		{
			assert(false);
		}
		else
		{
			cout << "havn't handle this: " << line << endl;
			assert(false);
		}
	}
	cout << "widht = " << G_WIDTH << "\theight = " << G_HEIGHT << endl;
	cout << "max depth = " << G_MAXDEPTH << endl;
	cout << "result file name:     " << G_OUTPUT_FILENAME << endl;
	cout << "camera look from:     " << G_CAM_LOOKFROM.x_ << ", " << G_CAM_LOOKFROM.y_ << ", " << G_CAM_LOOKFROM.z_ << endl
		 << "camera look at:       " << G_CAM_LOOKAT.x_   << ", " << G_CAM_LOOKAT.y_   << ", " << G_CAM_LOOKAT.z_ << endl
		 << "camera up direction:  " << G_CAM_UP.x_       << ", " << G_CAM_UP.y_       << ", " << G_CAM_UP.z_ << endl
		 << "camera field of view: " << G_FIELD_OF_VIEW << endl;
	cout << "point light: " << endl;
	for(list<Light>::const_iterator c_iter = G_POINTLIGHT_LIST.begin();
		c_iter != G_POINTLIGHT_LIST.end(); ++c_iter)
	{
		cout << "\tposition: (" << c_iter->origin_.x_ << ", " << c_iter->origin_.y_ << ", " << c_iter->origin_.z_ << ") "
			 << "\tcolor: (" << c_iter->color_.x_ << ", " << c_iter->color_.y_ << ", " << c_iter->color_.z_ << ")" << endl;
	}
	cout << "directional light: " << endl;
	for(list<Light>::const_iterator c_iter = G_DIRECTIONALLIGHT_LIST.begin();
		c_iter != G_DIRECTIONALLIGHT_LIST.end(); ++c_iter)
	{
		cout << "\tdirection: (" << c_iter->origin_.x_ << ", " << c_iter->origin_.y_ << ", " << c_iter->origin_.z_ << ") "
			 << "\tcolor: (" << c_iter->color_.x_ << ", " << c_iter->color_.y_ << ", " << c_iter->color_.z_ << ")" << endl;
	}
	// ATTENUATION
	cout << "Attenuation: " << G_ATTENUATION.x_ << ", " << G_ATTENUATION.y_ << ", " << G_ATTENUATION.z_ << endl;
	cout << "Sphere: " << endl;
	for(list<Sphere>::const_iterator c_iter = G_SPHERE_LIST.begin();
		c_iter != G_SPHERE_LIST.end(); ++c_iter)
	{
		cout << distance(G_SPHERE_LIST.cbegin(), c_iter) << ":" << ends
			 << "center: (" << c_iter->center_.x_ << ", " << c_iter->center_.y_ << ", " << c_iter->center_.z_ << "), radius: (" 
			 << c_iter->radius_ * c_iter->transform_mat_[0][0] << ", " << c_iter->radius_ * c_iter->transform_mat_[1][1] << ", " << c_iter->radius_ * c_iter->transform_mat_[2][2] << ")" << endl
			 << "ambient: " << c_iter->ambient_.r_ << ", " << c_iter->ambient_.g_ << ", " << c_iter->ambient_.b_ << endl
			 << "material:"   << endl
			 << "\tdiffuse_: "   << c_iter->material_.diffuse_.r_  << ", " << c_iter->material_.diffuse_.g_  << ", " << c_iter->material_.diffuse_.b_  << endl
			 << "\tspecular_: "  << c_iter->material_.specular_.r_ << ", " << c_iter->material_.specular_.g_ << ", " << c_iter->material_.specular_.b_ << endl
			 << "\tshininess_: " << c_iter->material_.shininess_   << endl
			 << "\temission_: "  << c_iter->material_.emission_.r_ << ", " << c_iter->material_.emission_.g_ << ", " << c_iter->material_.emission_.b_ << endl;
	}
	cout << "Triangle: " << endl;
	for(list<Triangle>::const_iterator c_iter = G_TRIANGLE_LIST.begin();
		c_iter != G_TRIANGLE_LIST.end(); ++c_iter)
	{
		cout << distance(G_TRIANGLE_LIST.cbegin(), c_iter) << ":" << ends
			 << "(" << c_iter->v1_.x_ << ", " << c_iter->v1_.y_ << ", " << c_iter->v1_.z_ << "), "
			 << "(" << c_iter->v2_.x_ << ", " << c_iter->v2_.y_ << ", " << c_iter->v2_.z_ << "), "
			 << "(" << c_iter->v3_.x_ << ", " << c_iter->v3_.y_ << ", " << c_iter->v3_.z_ << "), "<< endl
			 << "ambient: " << c_iter->ambient_.r_ << ", " << c_iter->ambient_.g_ << ", " << c_iter->ambient_.b_ << endl
			 << "material:"   << endl
			 << "\tdiffuse_: "   << c_iter->material_.diffuse_.r_  << ", " << c_iter->material_.diffuse_.g_  << ", " << c_iter->material_.diffuse_.b_  << endl
			 << "\tspecular_: "  << c_iter->material_.specular_.r_ << ", " << c_iter->material_.specular_.g_ << ", " << c_iter->material_.specular_.b_ << endl
			 << "\tshininess_: " << c_iter->material_.shininess_   << endl
			 << "\temission_: "  << c_iter->material_.emission_.r_ << ", " << c_iter->material_.emission_.g_ << ", " << c_iter->material_.emission_.b_ << endl;
	}
}
