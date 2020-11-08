#ifndef PATH_VERTICES_H
#define PATH_VERTICES_H
#include "Color.h"
#include "vec3.h"
#include "KD_Tree.h"
#include <math.h>
#include <vector>
#include "BXDF.h"
#include "Hit_rec.h"
#include "materials.h"
#include "Frame.h"
#include <cassert>
extern const float PI;



class Path_Vertex
{
public:
	Path_Vertex() {}
public:
	vec3 position;
	vec3 direction_world;
	vec3 throughput;

	float dVM;
};


class Path_Vertices
{
public:
	void init(int number_of_ligt_paths)
	{
		light_vertices.reserve(number_of_ligt_paths);
	}
	void build_kd_tree()
	{
		kd_tree.build_kd_tree(light_vertices);
	}
	void add_light_vertex(const Path_Vertex& vertex)
	{
		light_vertices.push_back(vertex);
	}

	vec3 get_contrib(const Isect& intersection, float radius_sqr, BxDF* camera_brdf, float camera_dVM)
	{

		std::vector<Path_Vertex> closest_light_vertices;
		kd_tree.find_closest_in_radius(intersection.position, radius_sqr, light_vertices, closest_light_vertices, 1);
		assert(!isnan(dVM));
		vec3 color(0.0f);

		for (int i = 0; i < closest_light_vertices.size(); ++i)
		{
			vec3 path_incomin_dir = closest_light_vertices[i].direction_world;
			float light_dVM = closest_light_vertices[i].dVM;
			float forward_pdf, reverse_pdf;
			camera_brdf->pdf(path_incomin_dir, forward_pdf, reverse_pdf);

			float weight = (1.0f / (1.0f + camera_dVM * reverse_pdf + light_dVM * forward_pdf));
			/*
			if (random_float_0_1() < 0.001f)
			{
				std::cout << weight << "  " << camera_dVM << " " << light_dVM << "\n";
			}*/
			color += camera_brdf->evaluate(path_incomin_dir, intersection.geometric_normal) * closest_light_vertices[i].throughput * weight;
		}
		assert(!isnan(color.x));
		assert(!isnan(color.y));
		assert(!isnan(color.z));
		return color;
	}
public:
	std::vector<Path_Vertex> light_vertices;
	KD_Tree kd_tree;

};




#endif
