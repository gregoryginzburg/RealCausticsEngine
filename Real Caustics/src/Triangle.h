#ifndef TRIANGLE_H
#define TRIANGLE_H 

class ray;
struct Isect;
class aabb;
class Material;
struct MVert;
class Transform;

class Triangle
{
public:
	Triangle() {}

	Triangle(unsigned int vert0_idx, unsigned int vert1_idx, unsigned int vert2_idx, unsigned int mat_idx) :
		vertex0_idx(vert0_idx), vertex1_idx(vert1_idx), vertex2_idx(vert2_idx), material_idx(mat_idx) {}

public:
	bool hit(const ray &r, float tmin, float tmax, Isect &hit_inf, MVert *vertices, const Transform& WorldTransformation, bool use_smooth_shading) const;
	
	aabb bounding_box(MVert *vertices, const Transform& WorldTransformation) const;

public:
	
	unsigned int vertex0_idx;
	unsigned int vertex1_idx;
	unsigned int vertex2_idx;

	unsigned int material_idx;

};
#endif
