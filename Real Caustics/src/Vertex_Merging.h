#ifndef VERTEX_MERGING_H
#define VERTEX_MERGING_H
//#include "Scene.h"
#include "Path_Vertices.h"
#include "Image_Buffer.h"
#include "BXDF.h"
class Scene;
class Light_Sub_Path_State;
class Camera_Sub_Path_State;
class Camera;
class MLT_Sampler;

struct Debug_stuff
{
	int number_of_points;
	vec3 direction;
	float roughness;
	float specular;
};

struct Debuggg_Stuff
{
	vec3 light_dir;
};

// Itegrator
class Vertex_Merging
{
public:
	Vertex_Merging() {}
public:
	void run();

	float run_iteration(int iteration, float mut_size);

	void trace_light_path(bool& visible, Path_Vertices& path_vertices, MLT_Sampler& Sampler, int index, Debuggg_Stuff& debug);

	void run_debug_normals();

	void run_debug_facing();

	void run_debug_materials();

	void run_debug_brdf();

	void run_debug_bvh();




public:

	Image_Buffer image_buffer;

	// Radius reduction rate
	float radius_alpha;
	// Initial merging radius
	float base_radius;
	// number of samples
	int samples;
	// maximum path length
	int max_path_length;

	Debug_stuff debug;

};


class Light_Sub_Path_State
{
public:
	vec3 origin;
	vec3 direction;
	vec3 throughput;
	
	int diffuse_interactions_count;
	BxDFType last_interaction;

	int path_length;

	float dVM;
};

class Camera_Sub_Path_State
{
public:
	vec3 origin;
	vec3 direction;
	vec3 throughput;

	int path_length;

	float dVM;
};

class Camera_State_Debug
{
public:
	vec3 origin;
	vec3 direction;
};

#endif // ! VERTEX_MERGING_H

