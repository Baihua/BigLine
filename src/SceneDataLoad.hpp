#ifndef _SceneDataLoad_H__
#define _SceneDataLoad_H__
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Triangle.hpp"

#include "Sphere.hpp"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_utils.hpp"
class SceneDataLoad {
public:
	SceneDataLoad() {}
	void Load(const char* file);
	Vector3f eyePos;
	float fov;
	std::map<std::string, Object*> objects;

	void strToV3(const std::string& str, Vector3f& v) {
		std::stringstream ss(str);
		ss >> v.x >> v.y >> v.z;
	}
};


void SceneDataLoad::Load(const char* file) {

	rapidxml::file<> fdoc(file);
	rapidxml::xml_document <> doc;
	doc.parse<0>(fdoc.data());

	rapidxml::xml_node<>* root = doc.first_node("Scene");
	rapidxml::xml_node<>* camera = root->first_node("camera");
	strToV3(camera->first_attribute("position")->value(), this->eyePos);
	this->fov = std::stof(camera->first_attribute("fov")->value());
	
	rapidxml::xml_node<>* objNode = root->first_node("obj");
	for (; objNode; objNode = objNode->next_sibling()) {
		char* type = objNode->first_attribute("ObjType")->value();
		std::string name(objNode->first_attribute("name")->value());
		if (std::strcmp(type, "mesh") == 0) {
			std::stringstream ss(objNode->first_node("indices")->value());
			int i;	std::vector<int> indices;
			while (ss>>i)
			{
				indices.push_back(i);
			}
			std::stringstream ss2(objNode->first_node("points")->value());
			float p;	std::vector<float> points;
			while (ss2 >> p)
			{
				points.push_back(p);
			}

			MeshTriangle* m = new MeshTriangle(indices, points);
			m->name = name;
			objects[name] = m;
		}
		else if (std::strcmp(type, "sphere") == 0) {
			char* cc = objNode->first_attribute("radius")->value();
			float r = std::stof(objNode->first_attribute("radius")->value());
			Vector3f p;
			strToV3(objNode->first_attribute("point")->value(),p);
			Sphere* sphere = new Sphere(p,r);
			objects[name] = sphere;
			sphere->name = name;
		}

	}

	//std::ifstream stream(file);
	//std::string str;

	//while (std::getline(stream, str))
	//{
	//	std::cout << str << std::endl;
	//	std::stringstream ss(str);
	//	std::string t;
	//	std::vector<int> indices;
	//	std::vector<float> point;
	//	bool nextIsIndex = false;
	//	std::string type;
	//	ss >> type;
	//	if (type == "mesh") {
	//		std::string name;
	//		ss >> name;
	//		while (ss >> t)
	//		{
	//			if (t == "indices:")
	//			{
	//				nextIsIndex = true;
	//				continue;
	//			}
	//			else if (t == "point:")
	//			{
	//				nextIsIndex = false;
	//				continue;
	//			}
	//			if (nextIsIndex)
	//			{
	//				int d = std::stoi(t);
	//				indices.push_back(d);
	//			}
	//			else {
	//				float f = std::stof(t);
	//				point.push_back(f);
	//			}
	//		}
	//		MeshTriangle* m = new MeshTriangle(indices, point);
	//		objects[name] = m;
	//	}
	//	else if (type == "sphere") {
	//		std::string name;
	//		float r; Vector3f pos;
	//		ss >> name;
	//		while (ss >> t)
	//		{
	//			if (t == "radius:")
	//			{
	//				ss >> r;
	//			}
	//			else if (t == "point:")
	//			{
	//				ss >> pos.x >> pos.y >> pos.z;
	//			}
	//		}
	//		Sphere* sp = new Sphere(pos, r);
	//		objects[name] = sp;
	//	}

	//}

}
#endif // !_SceneDataLoad_H__

