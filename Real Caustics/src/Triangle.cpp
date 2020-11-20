#include "Triangle.h"
#include "vec3.h"
#include "aabb.h"
#include "Hit_rec.h"
#include "Blender_definitions.h"
#include "Transform.h"

extern long long Triangle_Intersection_TESTS;


inline void normal_short_to_float(const short in[3], vec3& out)
{
    out.x = in[0] * (1.0f / 32767.0f);
    out.y = in[1] * (1.0f / 32767.0f);
    out.z = in[2] * (1.0f / 32767.0f);
}

//inline void apply_matrix_transformation(vec3& v, const matrix_4x4 

bool Triangle::hit(const ray& r, float tmin, float tmax, Isect& hit_inf, MVert *vertices, const Transform& WorldTransformation, bool use_smooth_shading) const
{
	++Triangle_Intersection_TESTS;

    vec3 vertice0 = vertices[vertex0_idx].co;
    vec3 vertice1 = vertices[vertex1_idx].co;
	vec3 vertice2 = vertices[vertex2_idx].co;

	vertice0 = WorldTransformation.ApplyToPoint(vertice0);
	vertice1 = WorldTransformation.ApplyToPoint(vertice1);
	vertice2 = WorldTransformation.ApplyToPoint(vertice2);

	vec3 vertex_normal0;
	vec3 vertex_normal1;
	vec3 vertex_normal2;

	normal_short_to_float(vertices[vertex0_idx].no, vertex_normal0);
	normal_short_to_float(vertices[vertex1_idx].no, vertex_normal1);
	normal_short_to_float(vertices[vertex2_idx].no, vertex_normal2);

	vertex_normal0 = normalize(WorldTransformation.ApplyToNormal(vertex_normal0));
	vertex_normal1 = normalize(WorldTransformation.ApplyToNormal(vertex_normal1));
	vertex_normal2 = normalize(WorldTransformation.ApplyToNormal(vertex_normal2));
	
	vec3 v0v1 = vertice1 - vertice0;
	vec3 v0v2 = vertice2 - vertice0;
	vec3 pvec = cross(r.direction, v0v2);
	float u, v;
	float t;

	float det = dot(v0v1, pvec);
	if (std::fabs(det) < 0.0000001f)
        return false;

	float invDet = 1.0f / det;

	vec3 tvec = r.origin - vertice0;
	u = dot(tvec, pvec) * invDet;
	if (u < 0.0f || u > 1.0f) return false;

	vec3 qvec = cross(tvec, v0v1);
	v = dot(r.direction, qvec) * invDet;
	if (v < 0.0f || u + v > 1.0f) 
        return false;

	t = dot(v0v2, qvec) * invDet;

	if (t > tmin && t < tmax)
	{
		hit_inf.distance = t;
		hit_inf.u = u;
		hit_inf.v = v;

		hit_inf.geometric_normal = normalize(cross(v0v1, v0v2));
		hit_inf.shade_normal = vertex_normal0 * (1 - u - v) + vertex_normal1 * u + vertex_normal2 * v;
		hit_inf.front_face = dot(hit_inf.shade_normal, r.direction) < 0 ? true : false;

		hit_inf.position = r.origin + r.direction * hit_inf.distance;
		hit_inf.direction = r.direction;
		hit_inf.material_idx = material_idx;

		return true;
	}
	else
    {
        return false;
    }	

}

aabb Triangle::bounding_box(MVert *vertices, const Transform& WorldTransformation) const
{
	vec3 vertice0 = vertices[vertex0_idx].co;
	vec3 vertice1 = vertices[vertex1_idx].co;
	vec3 vertice2 = vertices[vertex2_idx].co;

	vertice0 = WorldTransformation.ApplyToPoint(vertice0);
	vertice1 = WorldTransformation.ApplyToPoint(vertice1);
	vertice2 = WorldTransformation.ApplyToPoint(vertice2);

	return aabb(point3(std::min(std::min(vertice0.x, vertice1.x), vertice2.x),
		std::min(std::min(vertice0.y, vertice1.y), vertice2.y),
		std::min(std::min(vertice0.z, vertice1.z), vertice2.z)),

		point3(std::max(std::max(vertice0.x, vertice1.x), vertice2.x),
			std::max(std::max(vertice0.y, vertice1.y), vertice2.y),
			std::max(std::max(vertice0.z, vertice1.z), vertice2.z)));
}

