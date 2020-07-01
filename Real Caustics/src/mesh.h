#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include "hittable.h"
#include "vec2.h"
#include "materials.h"

class Triangle : public Hittable
{
public:
	Triangle() {}
	Triangle(point3 v0, point3 v1, point3 v2) : vertice0(v0), vertice1(v1), vertice2(v2) {}
	Triangle(point3 v0, point3 v1, point3 v2, vec2 t0, vec2 t1, vec2 t2, std::shared_ptr<Material> material, vec3 v_n0, vec3 v_n1, vec3 v_n2) : vertice0(v0), vertice1(v1), vertice2(v2), texture_vert0(t0), texture_vert1(t1), texture_vert2(t2), mat_ptr(material), vertex_normal0(v_n0), vertex_normal1(v_n1), vertex_normal2(v_n2) {}
public:
	inline vec3 getNormal() const
	{
		return normalize(cross(vertice1 - vertice0, vertice2 - vertice0));
	}
	virtual bool hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const
	{
		vec3 v0v1 = vertice1 - vertice0;
		vec3 v0v2 = vertice2 - vertice0;
		vec3 pvec = cross(r.direction, v0v2);
		double u;
		double v;
		double t;
		double det = dot(v0v1, pvec);
		if (std::fabs(det) < 0.0000001) return false;

		double invDet = 1.0 / det;

		vec3 tvec = r.origin - vertice0;
		u = dot(tvec, pvec) * invDet;
		if (u < 0.0 || u > 1.0) return false;

		vec3 qvec = cross(tvec, v0v1);
		v = dot(r.direction, qvec) * invDet;
		if (v < 0.0 || u + v > 1.0) return false;

		t = dot(v0v2, qvec) * invDet;
		
		if (t > tmin && t < tmax)
		{
			hit_inf.t = t;
			hit_inf.u = u;
			hit_inf.v = v;
			hit_inf.normal = vertex_normal0 * (1 - u - v) + vertex_normal1 * u + vertex_normal2 * v;
			hit_inf.front_face = dot(hit_inf.normal, r.direction) < 0 ? 1 : 0;
			hit_inf.p = r.origin + r.direction * hit_inf.t;
			hit_inf.mat_ptr = mat_ptr;
			hit_inf.tex_coord_v0 = texture_vert0;
			hit_inf.tex_coord_v1 = texture_vert1;
			hit_inf.tex_coord_v2 = texture_vert2;
			return true;
		}
		else
			return false;

	}

	virtual bool bounding_box(aabb& output_box) const
	{
		output_box = aabb(point3(std::fmin(std::fmin(vertice0.x, vertice1.x), vertice2.x),
				std::fmin(std::fmin(vertice0.y, vertice1.y), vertice2.y),
				std::fmin(std::fmin(vertice0.z, vertice1.z), vertice2.z)),

			point3(std::fmax(std::fmax(vertice0.x, vertice1.x), vertice2.x),
				std::fmax(std::fmax(vertice0.y, vertice1.y), vertice2.y),
				std::fmax(std::fmax(vertice0.z, vertice1.z), vertice2.z)));
		return true;
	}
public:
	point3 vertice0;
	point3 vertice1;
	point3 vertice2;
	vec2 texture_vert0;
	vec2 texture_vert1;
	vec2 texture_vert2;
	std::shared_ptr<Material> mat_ptr;
	vec3 vertex_normal0;
	vec3 vertex_normal1;
	vec3 vertex_normal2;

};


class Mesh : public Hittable
{
public:
	Mesh() {}
	Mesh(std::vector<std::shared_ptr<Hittable>> tris) : triangles(tris) {}

public:
	int number_of_tris()
	{
		return triangles.size();
	}
	void add(std::shared_ptr<Hittable> object)
	{
		triangles.push_back(object);
	}

	virtual bool hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const
	{
		return true;
	}
	virtual bool bounding_box(aabb& output_box) const
	{
		return true;
	}

public:
	std::vector<std::shared_ptr<Hittable>> triangles;
};

#endif
