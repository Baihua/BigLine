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
#include "DiffuseBxdf.hpp"
#include "EmssionBxdf.hpp"
#include "MirrorBxdf.hpp"
#include "MetalBxdf.hpp"
#include "Light.hpp"
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
	//sl.Load("./scene/mis2.xml");
	//Material* metal = new Material(MICROFACET, Vector3f(0.0f));
	//metal->ior = 20;
	//metal->Roughness = 0.3;
	//Material* red = new Material(DIFFUSE, Vector3f(0.0f));
	//red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
	//Material* green = new Material(DIFFUSE, Vector3f(0.0f));
	//green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
	//Material* white = new Material(DIFFUSE, Vector3f(0.0f));
	//white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
	//Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	//light->Kd = Vector3f(0.65f);





	//MateialGlass* glass = new MateialGlass();
	//glass->reflectance = Vector3f(0.9f);
	//glass->ior = 1.5f;

	Material* green = new Material(DIFFUSE, Vector3f(0.0f));
	DiffuseBxdf* green_ = new DiffuseBxdf();
	green_->Kd = Vector3f(0.14f, 0.45f, 0.091f);
	green->bxdf = green_;

	Material* red = new Material(DIFFUSE, Vector3f(0.0f));
	DiffuseBxdf* red_ = new DiffuseBxdf();
	red_->Kd = Vector3f(0.63f, 0.065f, 0.05f);// Vector3f(1.0f, 0.0f, 0.0f);
	red->bxdf = red_;

	Material* white = new Material(DIFFUSE, Vector3f(0.0f));
	DiffuseBxdf* diffus = new DiffuseBxdf();
	diffus->Kd = Vector3f(0.725f, 0.71f, 0.68f);
	white->bxdf = diffus;

	MateialMirror* mirror = new MateialMirror();
	MirrorBxdf* mirror_ = new MirrorBxdf();
	mirror_->reflectance = Vector3f(0.9);
	mirror->bxdf = mirror_;

	Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	EmissionBxdf* emission = new EmissionBxdf();
	emission->emission = (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));
	light->bxdf = emission;


	Material* lightRed = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	EmissionBxdf* emissionRed = new EmissionBxdf();
	emissionRed->emission = 20.0f * Vector3f(1, 0, 0);
	//light->color = 20.0f * Vector3f(1, 0, 0);
	lightRed->bxdf = emissionRed;


	Material* lightGreen = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	EmissionBxdf* emissionGreen = new EmissionBxdf();
	emissionGreen->emission = (500 * Vector3f(0, 1, 0));
	lightGreen->bxdf = emissionGreen;

	Material* lightYellow = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
	EmissionBxdf* emissionYellow = new EmissionBxdf();
	emissionYellow->emission = (200.0f * Vector3f(0.0f, 0, 1.0));
	lightYellow->bxdf = emissionYellow;

	Material* metal1 = new Material();
	Microfacet* mf1 = new Microfacet();
	mf1->reflectance = Vector3f(0.8, 0.8, 0.8);
	mf1->ior = 20;
	mf1->SetRoughness(0.1);
	metal1->bxdf = mf1;

	Material* metal2 = new Material();
	Microfacet* mf2 = new Microfacet();
	mf2->reflectance = Vector3f(0.8, 0.8, 0.8);
	mf2->ior = 20;
	mf2->SetRoughness(0.2);
	metal2->bxdf = mf2;

	Material* metal3 = new Material();
	Microfacet* mf3 = new Microfacet();
	mf3->reflectance = Vector3f(0.8, 0.8, 0.8);
	mf3->ior = 20;
	mf3->SetRoughness(0.3);
	metal3->bxdf = mf3;

	Material* metal4 = new Material();
	Microfacet* mf4 = new Microfacet();
	mf4->reflectance = Vector3f(0.8, 0.8, 0.8);
	mf4->ior = 20;
	mf4->SetRoughness(0.4);
	metal4->bxdf = mf4;

	sl.Load("./scene/mis.xml");
	sl.objects["floor"]->SetMaterial(white);
	sl.objects["back"]->SetMaterial(white);

	sl.objects["p1"]->SetMaterial(metal1);
	sl.objects["p2"]->SetMaterial(metal2);
	sl.objects["p3"]->SetMaterial(metal3);
	sl.objects["p4"]->SetMaterial(metal4);

	sl.objects["small"]->SetMaterial(lightGreen);
	DiffuseLight* light1 = new DiffuseLight();
	light1->obj = sl.objects["small"];
	light1->color = (500 * Vector3f(1, 1, 1));
	sl.objects["small"]->light = light1;

	sl.objects["middle"]->SetMaterial(lightYellow);
	DiffuseLight* light2 = new DiffuseLight();
	light2->obj = sl.objects["middle"];
	light2->color = (200.0f * Vector3f(1.0f, 1, 1.0));
	sl.objects["middle"]->light = light2;

	sl.objects["big"]->SetMaterial(lightRed);
	DiffuseLight* light3 = new DiffuseLight();
	light3->obj = sl.objects["big"];
	light3->color = 20.0f * Vector3f(1, 1, 1);
	sl.objects["big"]->light = light3;

	//sl.Load("./scene/mis2.xml");
	//sl.objects["floor"]->SetMaterial(red);
	//sl.objects["middle"]->SetMaterial(light);
	//DiffuseLight* ll = new DiffuseLight();
	//ll->obj = sl.objects["middle"];
	//ll->color = (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));
	//sl.objects["middle"]->light = ll;
	//sl.objects["mr"]->SetMaterial(metal1);
	//sl.objects["df"]->SetMaterial(green)/**/;

	//sl.Load("./scene/cornell.xml");

	//sl.objects["floor"]->SetMaterial(white);
	//sl.objects["ceiling"]->SetMaterial(white);
	//sl.objects["back"]->SetMaterial(white);
	//sl.objects["left"]->SetMaterial(red);
	//sl.objects["right"]->SetMaterial(green);
	//sl.objects["light"]->SetMaterial(light);
	//DiffuseLight* ll = new DiffuseLight();
	//ll->obj = sl.objects["light"];
	//ll->color = (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f));
	//sl.objects["light"]->light = ll;
	//sl.objects["tallBox"]->SetMaterial(white);
	//sl.objects["shortBox"]->SetMaterial(white);


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

	scene.eyePos = sl.eyePos;
	scene.fov = sl.fov;
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
	Material* metal = new Material(MICROFACET, Vector3f(0.0f/
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