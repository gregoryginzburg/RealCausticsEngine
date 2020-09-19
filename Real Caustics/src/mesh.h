#ifndef MESH_H
#define MESH_H

#include <vector>
#include "BVH\BVH_mesh.h"
#include "matrix.h"


struct Mesh_blender;
class ray;
struct hit_rec;
struct MVert;
class Triangle;
struct CacheBVHNode_mesh;
class Object_Materials;


extern const float inf;

class Mesh
{
public:
	Mesh() {}

	Mesh(Mesh_blender *mesh_blender_data, unsigned int number_of_vertices, unsigned int number_of_tris, const matrix_4x4& matrix,
		int* material_indices);


public:
	// defined in BVH_mesh.cpp
	bool hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, int index) const;

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
	matrix_4x4 world_matrix;
};

#endif
