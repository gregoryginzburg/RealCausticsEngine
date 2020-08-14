#include "Triangle.h"
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include "vec3.h"
#include "Hit_rec.h"
#include "vec2.h"
#include "materials.h"
#include "aabb.h"
#include "Scene.h"
#include "Blender_definitions.h"

inline float* normal_short_to_float(const short in[3])
{
    float out[3];
    out[0] = in[0] * (1.0f / 32767.0f);
    out[1] = in[1] * (1.0f / 32767.0f);
    out[2] = in[2] * (1.0f / 32767.0f);
    return out;
}

bool Triangle::hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf, MVert *vertices) const 
{
    vec3 vertice0 = vertices[vertex0_idx].co;
    vec3 vertice1 = vertices[vertex1_idx].co;
	vec3 vertice2 = vertices[vertex2_idx].co;

    vec3 vertex_normal0 = normal_short_to_float(vertices[vertex0_idx].no);
    vec3 vertex_normal1 = normal_short_to_float(vertices[vertex1_idx].no);
	vec3 vertex_normal2 = normal_short_to_float(vertices[vertex2_idx].no);

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
		hit_inf.t = t;
		hit_inf.u = u;
		hit_inf.v = v;
		hit_inf.normal = vertex_normal0 * (1 - u - v) + vertex_normal1 * u + vertex_normal2 * v;
		hit_inf.front_face = dot(hit_inf.normal, r.direction) < 0 ? true : false;
		hit_inf.p = r.origin + r.direction * hit_inf.t;
		hit_inf.material_idx = material_idx;

		// hit_inf.tex_coord_v0 = 0;
		// hit_inf.tex_coord_v1 = 0;
		// hit_inf.tex_coord_v2 = 0;
		return true;
	}
	else
    {
        return false;
    }		

}

aabb Triangle::bounding_box(MVert *vertices) const
{
	vec3 vertice0 = vertices[vertex0_idx].co;
	vec3 vertice1 = vertices[vertex1_idx].co;
	vec3 vertice2 = vertices[vertex2_idx].co;

	return aabb(point3(std::fmin(std::fmin(vertice0.x, vertice1.x), vertice2.x),
		std::fmin(std::fmin(vertice0.y, vertice1.y), vertice2.y),
		std::fmin(std::fmin(vertice0.z, vertice1.z), vertice2.z)),

		point3(std::fmax(std::fmax(vertice0.x, vertice1.x), vertice2.x),
			std::fmax(std::fmax(vertice0.y, vertice1.y), vertice2.y),
			std::fmax(std::fmax(vertice0.z, vertice1.z), vertice2.z)));
}






Triangle::~Triangle()
{

} 