#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>
#include "MateialMirror.hpp"
#include "MateialGlass.hpp"
#include "SceneDataLoad.hpp"

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
std::mutex m;
int main(int argc, char** argv)
{

	// Change the definition here to change resolution
	//Scene scene(784, 784);
	Scene scene(512, 512);
	//Scene scene(256, 256);
	SceneDataLoad sl;
	//sl.Load("./scene/cornell-box.txt");
	sl.Load("./scene/mis2.txt");

	Material* red = new Material(DIFFUSE, Vector3f(0.0f));
	red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
	Material* green = new Material(DIFFUSE, Vector3f(0.0f));
	green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
	Material* white = new Material(DIFFUSE, Vector3f(0.0f));
	white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
	Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	light->Kd = Vector3f(0.65f);
	MateialMirror* mirror = new MateialMirror();
	mirror->reflectance = Vector3f(0.9f);

	MateialGlass* glass = new MateialGlass();
	glass->reflectance = Vector3f(0.9f);
	glass->ior = 1.5f;

	sl.objects["floor"]->SetMaterial(white);
	/*sl.objects["back"]->SetMaterial(white);
	
	sl.objects["p1"]->SetMaterial(white);
	sl.objects["p2"]->SetMaterial(white);
	sl.objects["p3"]->SetMaterial(white);
	sl.objects["p4"]->SetMaterial(white);

	sl.objects["small"]->SetMaterial(light)*/;
	sl.objects["middle"]->SetMaterial(light);
	sl.objects["mr"]->SetMaterial(mirror);
	sl.objects["df"]->SetMaterial(green);
	//sl.objects["big"]->SetMaterial(light);

	for (auto item : sl.objects) {
		scene.Add(item.second);
	}
	//std::vector<int> ind{ 0, 1, 2, 0, 2, 3 }; std::vector<Vector3f> v{ Vector3f(0.0, 0.0, 559.2),Vector3f(0.0, 0.0, 0.0), Vector3f(0.0, 548.8, 0.0), Vector3f(0.0, 548.8, 559.2) };
	//MeshTriangle floor("./models/cornellbox/floor.obj", white);
	//MeshTriangle shortbox("./models/cornellbox/shortbox.obj", white);
	//MeshTriangle tallbox("./models/cornellbox/tallbox.obj", white);
	//MeshTriangle left("./models/cornellbox/left.obj", red);
	////MeshTriangle right("./models/cornellbox/right.obj", green);
	//MeshTriangle right(ind, v, green);
	//MeshTriangle light_("./models/cornellbox/light.obj", light);


	//MeshTriangle mirrorTallbox("./models/cornellbox/tallbox.obj", mirror);
	//MeshTriangle mirrorShortbox("./models/cornellbox/shortbox.obj", mirror);

	//MeshTriangle glassShortbox("./models/cornellbox/shortbox.obj", glass);

	////测试mirror grass
	//Sphere* sp = new Sphere(Vector3f(150, 100, 200), 100, glass);
	//scene.Add(&floor);
	//scene.Add(&tallbox);
	//scene.Add(sp);
	////scene.Add(&glas);
	//scene.Add(&left);
	//scene.Add(&right);
	//scene.Add(&light_);


	 ////任务1测试场景
	 //scene.Add(&floor);
	 //scene.Add(&shortbox);
	 //scene.Add(&tallbox);
	 //scene.Add(&left);
	 //scene.Add(&right);
	 //scene.Add(&light_);
	//MeshTriangle* floor = sl.meshes["floor"];
	//floor->SetMaterial(white);
	//MeshTriangle* ceiling = sl.meshes["ceiling"];
	//ceiling->SetMaterial(white);
	//MeshTriangle* backwall = sl.meshes["back"];
	//backwall->SetMaterial(white);
	//MeshTriangle* leftWall = sl.meshes["left"];
	//leftWall->SetMaterial(red);
	//MeshTriangle* rightWall = sl.meshes["right"];
	//rightWall->SetMaterial(green);
	//MeshTriangle* light_ = sl.meshes["light"];
	//light_->SetMaterial(light);

	//MeshTriangle* tallBox = sl.meshes["tallBox"];
	//tallBox->SetMaterial(white);
	//MeshTriangle* shortBox = sl.meshes["shortBox"];
	//shortBox->SetMaterial(white);

	scene.eyePos = Vector3f(0,-1.1,-7.17);
	//scene.Add(floor);
	//scene.Add(backwall);
	//scene.Add(leftWall);
	//scene.Add(rightWall);
	//scene.Add(ceiling);
	//scene.Add(light_);
	//scene.Add(tallBox);
	//scene.Add(shortBox);
	//测试mirror
   //scene.Add(&floor);
   //scene.Add(&mirrorTallbox);
   //scene.Add(&mirrorShortbox);
   //scene.Add(&left);
   //scene.Add(&right);
   //scene.Add(&light_);


   /* //测试microfacet 场景
	Material* metal = new Material(MICROFACET, Vector3f(0.0f));
	metal->ior = 20;
	metal->Roughness = 0.8;
	Sphere *sp = new Sphere(Vector3f(100, 100, 300), 100, metal);
	Sphere* sp2 = new Sphere(Vector3f(400, 100, 300), 100, white);
	scene.Add(&floor);
	scene.Add(&left);
	scene.Add(&right);
	scene.Add(sp);
	scene.Add(sp2);
	scene.Add(&light_);
  */

	scene.buildBVH();

	Renderer r;

	auto start = std::chrono::system_clock::now();
	r.SetSomeSetting(64, 4);//设置SPP与线程数
	r.Render(scene);
	auto stop = std::chrono::system_clock::now();

	std::cout << "Render complete: \n";
	std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
	std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
	std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
	system("pause");
	return 0;
}