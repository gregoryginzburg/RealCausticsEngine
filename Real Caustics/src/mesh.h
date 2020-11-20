#ifndef MESH_H
#define MESH_H

#include <vector>
#include "BVH\BVH_mesh.h"
#include "matrix.h"
#include "Transform.h"


struct Mesh_blender;
class ray;
struct Isect;
struct MVert;
class Triangle;
struct CacheBVHNode_mesh;
class Object_Materials;


extern const float inf;

class Mesh
{
public:
	Mesh() {}

	Mesh(Mesh_blender *mesh_blender_data, unsigned int number_of_vertices, unsigned int number_of_tris, const Matrix4x4& matrix,
		int* material_indices, bool smooth_shade);


public:
	// defined in BVH_mesh.cpp
	bool hit(const ray &r, float tmin, float tmax, Isect &hit_inf, int index) const;

	BVHNode_mesh *make_default_bvh();

	void update_bvh(bool was_changed, const char *file_path);

public:
	unsigned int number_of_triangles;
	//array of blender_vertices
	MVert *vertices;

	//array of triangles
	Triangle *triangles;

	BVH_mesh BVH;
	aabb bounding_box;
	Transform WorldTransformation;


	bool shade_smooth;
};

#endif
