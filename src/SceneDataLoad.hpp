#ifndef _SceneDataLoad_H__
#define _SceneDataLoad_H__
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Triangle.hpp"
class SceneDataLoad {
public:
	SceneDataLoad() {}
	void Load(const char* file);

	std::map<std::string, MeshTriangle*> meshes;
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
		std::string name;
		ss >> name;
		while (ss >> t)
		{
			if (t == "indices:")
			{
				nextIsIndex = true;
				continue;
			}else if(t=="point:")
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
			std::cout << t << std::endl;
		}
		MeshTriangle* m = new MeshTriangle(indices, point);
		meshes[name] = m;
	}

}
#endif // !_SceneDataLoad_H__

