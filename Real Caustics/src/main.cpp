#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include <cmath>
#include "vec3.h"
#include "timer.h"
#include "mesh.h"
#include "Triangle.h"
#include "ray.h"
#include "utils.h"
#include "Color.h"
#include "parser.h"
#include "Lights.h"
#include "matrix.h"
#include "BVH\BVH_mesh.h"
#include "Camera.h"
#include "DLL.h"
#include "Blender_definitions.h"
#include "Scene.h"


#define TINYEXR_IMPLEMENTATION
#include "../lib/tinyexr.h"


//#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

#define REPORT_PROGRESS
extern std::ofstream test_out = std::ofstream();




extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float E = 2.71828182846;
extern const float INV_E = 0.36787944117;
extern const float PI2 = 6.28318530718f;
extern const float Inv_PI = 0.31830988618;
extern const float PI_OVER_2 = 1.57079632679;
extern const float PI_OVER_4 = 0.78539816339;

extern long long Triangle_Intersection_TESTS = 0;

extern long long BVH_Intersection_TESTS = 0;

//-------------------------------------------
//----------------SCENE----------------------
//extern Scene *scene = nullptr;
extern Scene* scene = nullptr;
Python_Light* python_lights = nullptr;

//----------------SCENE----------------------
//-------------------------------------------


// :samples - number of samples
// :base_radius - starting radius
// :radius_reduction - radius reduction
// :mesh_pointers - array of pointers to blender meshes
// :number_of_meshes
// :mesh_matrices - matrix_world matrices 
// :meshes_number_of_verts - array of number of vertices
// :meshes_number_of_tris - array of number of triangles 
// :camera_x - camera width resolution
// :camera_y - camera height resolution  
// :camera_position
// :camera_corner 0,1,2,3 - top right, bottom right, bottom left, top left corners of camera
//  4----1
//	|    |  - Camera corners
//  |    |
//  3----2
// :lights - Lights from Python, need to be converted
// :number_of_lights
// :materials - Materials from Python, need to be converted 
// :number_of_materials
// :materials_indices - triangles have material indices but they are tied to materials linked to mesh - array of material indices (length = number of meshes)
// :meshes_material_idx - Array with length of number_of_meshes, indices of materials

DLLEXPORT void init(int samples, float base_radius, float radius_reduction,
					long long *meshes_pointers,
					unsigned int number_of_meshes,
					matrix_4x4* mesh_matrices,
					unsigned int *meshes_number_of_verts,
					unsigned int *meshes_number_of_tris,
					unsigned int camera_x,
					unsigned int camera_y,
					vec3 camera_position,
					vec3 camera_corner0,
					vec3 camera_corner1,
					vec3 camera_corner2,
					vec3 camera_corner3,
					Python_Light *lights,
					unsigned int number_of_lights,
					Python_Material *materials,
					unsigned int number_of_materials,
					int** materials_indices,
					int debug_test,
					Debug_stuff debug_stuff)
{
	scene = new Scene;
	//initialize common photon mapping settings
	scene->integrator.max_path_length = 7;
	scene->integrator.samples = samples;
	scene->integrator.base_radius = base_radius;
	scene->integrator.radius_alpha = radius_reduction;
	scene->integrator.debug = debug_stuff;

	//initialize meshes with blender data
	scene->number_of_meshes = number_of_meshes;

	scene->init_meshes(meshes_pointers, meshes_number_of_verts, meshes_number_of_tris, mesh_matrices, materials_indices);

	//initialize camera
	scene->camera = Camera(camera_x, camera_y,
						   camera_position,
						   camera_corner0,
						   camera_corner1,
						   camera_corner2,
						   camera_corner3);

	//initialize lights and calculate weights for photon emission
	scene->number_of_lights = number_of_lights;
	scene->lights.light_count = number_of_lights;
	python_lights = lights;

	//initialize materials
	scene->number_of_materials = number_of_materials;

	scene->init_materials(materials);

	scene->debug_test = debug_test;
	//std::cout << *scene;
}
using std::cos;
using std::sin;


DLLEXPORT int main()
{
	Timer summary_time;
	Triangle_Intersection_TESTS = 0;
	BVH_Intersection_TESTS = 0;

	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;

	scene->update_bvh(1, "test_scene_bvh");

	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;

	// 
	// scene bounding bbox is availibale adter building bvh
	scene->init_lights(python_lights);
	scene->lights.update_pdf_distribution();
	
	Timer rendering;

	if (scene->debug_test == 1)
		scene->integrator.run();
	else if (scene->debug_test == 2)
		scene->integrator.run_debug_normals();
	else if (scene->debug_test == 3)
		scene->integrator.run_debug_facing();
	else if (scene->debug_test == 4)
		scene->integrator.run_debug_materials();
	else if (scene->debug_test == 5)
		scene->integrator.run_debug_bvh();
		
	std::cout << "Rendering took:    " << rendering.elapsed();
	//scene->integrator.run_debug_brdf();

	
	int count = scene->camera.pixel_width * scene->camera.pixel_height * 3;
	float* image = new float[count];
	int j = 0;
	for (int i = 0; i < count; i+=3)
	{
		image[i] = scene->integrator.image_buffer.color[j].x;
		image[i + 1] = scene->integrator.image_buffer.color[j].y;
		image[i + 2] = scene->integrator.image_buffer.color[j].z;
		++j;
	}

	const char* err = nullptr;
	SaveEXR(image, scene->camera.pixel_width, scene->camera.pixel_height, 3, 0, "D:\\caustics.exr", &err);
	
#ifdef REPORT_PROGRESS

	std::cout << "\n";
	std::cout << "Summary Time  :  " << summary_time.elapsed() << std::endl;
	std::cout << "STATICITICS:    \n";
	std::cout << "Triangle Intersection Tests:         " << Triangle_Intersection_TESTS << "\n";
	std::cout << "BVH Intersection Tests:         " << BVH_Intersection_TESTS << "\n";

#endif
	test_out.close();
	delete scene;
	scene = nullptr;
	return 0;
}
