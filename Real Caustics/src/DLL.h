#ifndef _DLL_H_
#define _DLL_H_
#include <iostream>
#define DLLEXPORT extern "C" __declspec(dllexport)


#ifdef  __cplusplus
extern "C"
{
    #endif


    DLLEXPORT int main();
	DLLEXPORT void init(int number_of_photons, int n_closest, float radius,
		long long* meshes_pointers,
		unsigned int number_of_meshes,
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
		int* meshes_material_idx);


    #ifdef  __cplusplus
}
#endif
#endif

