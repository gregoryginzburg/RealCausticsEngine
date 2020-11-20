#ifndef _DLL_H_
#define _DLL_H_
#include <iostream>
#define DLLEXPORT extern "C" __declspec(dllexport)
struct Debug_stuff;

#ifdef  __cplusplus
extern "C"
{
    #endif


    DLLEXPORT int main();
	DLLEXPORT void init(int samples, float base_radius, float radius_reduction,
		long long* meshes_pointers,
		unsigned int number_of_meshes,
		PythonMatrix4x4* mesh_matrices,
		unsigned int* meshes_number_of_verts,
		unsigned int* meshes_number_of_tris,
		unsigned int camera_x,
		unsigned int camera_y,
		vec3 camera_position,
		vec3 camera_corner0,
		vec3 camera_corner1,
		vec3 camera_corner2,
		vec3 camera_corner3,
		Python_Light* lights,
		unsigned int number_of_lights,
		Python_Material* materials,
		unsigned int number_of_materials,
		int** materials_indices,
		int debug_test,
		Debug_stuff debug_stuff);


    #ifdef  __cplusplus
}
#endif
#endif

