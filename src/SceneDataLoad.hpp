#ifndef _SceneDataLoad_H__
#define _SceneDataLoad_H__
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Triangle.hpp"

#include "Sphere.hpp"

class SceneDataLoad {
public:
	SceneDataLoad() {}
	void Load(const char* file);

	std::map<std::string, Object*> objects;
};

void SceneDataLoad::Load(const char* file) {

	std::ifstream stream(file);
	std::string str;

	while (std::getline(stream, str))
	{
		std::cout << str << std::endl;
		std::stringstream ss(str);
		std::string t;
		std::vector<int> indices;
		std::vector<float> point;
		bool nextIsIndex = false;
		std::string type;
		ss >> type;
		if (type == "mesh") {
			std::string name;
			ss >> name;
			while (ss >> t)
			{
				if (t == "indices:")
				{
					nextIsIndex = true;
					continue;
				}
				else if (t == "point:")
				{
					nextIsIndex = false;
					continue;
				}
				if (nextIsIndex)
				{
					int d = std::stoi(t);
					indices.push_back(d);
				}
				else {
					float f = std::stof(t);
					point.push_back(f);
				}
			}
			MeshTriangle* m = new MeshTriangle(indices, point);
			objects[name] = m;
		}
		else if (type == "sphere") {
			std::string name;
			float r; Vector3f pos;
			ss >> name;
			while (ss >> t)
			{
				if (t == "radius:")
				{
					ss >> r;
				}
				else if (t == "point:")
				{
					ss >> pos.x >> pos.y >> pos.z;
				}
			}
			Sphere* sp = new Sphere(pos, r);
			objects[name] = sp;
		}

	}

}
#endif // !_SceneDataLoad_H__

