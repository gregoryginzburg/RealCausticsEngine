#ifndef TRIANGLE_H
#define TRIANGLE_H 

class ray;
struct hit_rec;
class aabb;
class Material;
struct MVert;
class matrix_4x4;

class Triangle
{
public:
	Triangle() {}

	Triangle(unsigned int vert0_idx, unsigned int vert1_idx, unsigned int vert2_idx, unsigned int mat_idx) :
		vertex0_idx(vert0_idx), vertex1_idx(vert1_idx), vertex2_idx(vert2_idx), material_idx(mat_idx) {}
	~Triangle();

public:
	bool hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, MVert *vertices, const matrix_4x4& world_matrix) const;
	
	aabb bounding_box(MVert *vertices, const matrix_4x4& world_matrix) const;

public:
	
	unsigned int vertex0_idx;
	unsigned int vertex1_idx;
	unsigned int vertex2_idx;

	unsigned int material_idx;

};
#endif
