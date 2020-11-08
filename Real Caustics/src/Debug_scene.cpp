#include "Vertex_Merging.h"
#include "Scene.h"
#include "vec3.h"
#include "ray.h"
#include "BXDF.h"
#include "Path_Vertices.h"
#include "Camera.h"
#include <cmath>
#include <fstream>
#include "BVH\BVH_mesh.h"
#include "BVH\BVH_world.h"
#include "mesh.h"


extern const float PI;
extern const float inf;
extern Scene* scene;


void Vertex_Merging::run_debug_normals()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;


	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			image_buffer.add_color(sample, hit_inf.geometric_normal);
			break;
		}

	}
}

void Vertex_Merging::run_debug_facing()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;


	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			vec3 color;
			if (hit_inf.front_face)
				color = vec3(0, 0, 1);
			else
				color = vec3(1, 0, 0);

			float factor = std::abs(dot(r.direction, hit_inf.shade_normal));
			image_buffer.add_color(sample, color * factor);
			break;
		}

	}
}


void Vertex_Merging::run_debug_brdf()
{
	
	Isect intersection;
	intersection.geometric_normal = vec3(0, 0, 1); 
	intersection.shade_normal = vec3(0, 0, 1);
	intersection.direction = -debug.direction;

	BxDF* brdf = new Fresnel_Microfacet_GGX(vec3(1, 1, 1), debug.roughness * debug.roughness, debug.specular * 0.08, intersection, TransportMode::Importance);
	Fresnel_Microfacet_GGX* new_brdf = dynamic_cast<Fresnel_Microfacet_GGX*>(brdf);

	std::ofstream o;
	o.open("D:\\BRDF\\brdf.obj");

	std::ofstream normals;
	normals.open("D:\\BRDF\\normals.obj");

	std::ofstream all_directions;
	all_directions.open("D:\\BRDF\\all_directions.obj");

	std::ofstream scaled_by_pdf;
	scaled_by_pdf.open("D:\\BRDF\\scaled_by_pdf.obj");

	float f_pdf;
	float r_pdf;
	float cos_thet_out;
	BxDFType sampled_event;

	for (int i = 0; i < debug.number_of_points; ++i)
	{
		vec3 wi;
		vec3 half_vector;
		//vec3 color = new_brdf->sample(wi, intersection.geometric_normal, f_pdf, r_pdf, cos_thet_out, sampled_event);

		all_directions << "v " << wi.x << " " << wi.y << " " << wi.z << "\n";

		//wi *= color;
		o << "v " << wi.x << " " << wi.y << " " << wi.z << "\n";
		normals << "v " << half_vector.x << " " << half_vector.y << " " << half_vector.z << "\n";

	}


}


void Vertex_Merging::run_debug_materials()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;

	
	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			vec3 color;

			Frame frame;
			frame.set_from_z(hit_inf.shade_normal);
			vec3 dir_local = frame.to_local(-hit_inf.direction);

			float ior = debug.specular;
			float etai_over_etat = ior;
			// if entering from normal side(in the substance)
			//if (hit_inf.front_face)
				//etai_over_etat = 1.0f / ior;

			//float u = random_float(0.0f, 1.0f);
			float f = 0.0f;//fresnel_dielectric_cos(std::abs(dir_local.z), etai_over_etat);

			//float factor = std::abs(dot(r.direction, hit_inf.normal));
			image_buffer.add_color(sample, f);
			break;
		}

	}
}
inline std::ofstream& write_vec3(std::ofstream& out, const vec3& a)
{
	out << "v " << a.x << " " << a.y << " " << a.z << "\n";
	return out;
}

void Vertex_Merging::run_debug_bvh()
{
	std::ofstream meshes;
	meshes.open("D:\\bvh\\meshes.obj");

	std::ofstream triangles;
	triangles.open("D:\\bvh\\triangles.obj");

	auto& bvh = scene->BVH;


	for (int i = 0; i < bvh.bvh_nodes.size(); ++i)
	{
		auto& bvh_node = bvh.bvh_nodes[i];
		vec3 min_world = bvh_node.bounding_box.min;
		vec3 max_world = bvh_node.bounding_box.max;

		int end_index = bvh_node.u.leaf.startIndex + (bvh_node.u.leaf.count & 0x7fffffff) + 1;
		for (int i = bvh_node.u.leaf.startIndex + 1; i < end_index; ++i)
		{
			BVH_mesh& bvh_mesh = scene->meshes[bvh.mesh_indices[i]].BVH;
			for (int j = 0; j < bvh_mesh.bvh_nodes.size(); ++j)
			{
				auto& bvh_mesh_node = bvh_mesh.bvh_nodes[j];
				vec3 min_mesh = bvh_mesh_node.bounding_box.min;
				vec3 max_mesh = bvh_mesh_node.bounding_box.max;

				//triangles << "v " << min_mesh.x << " " << min_mesh.y << " " << min_mesh.z << "\n";
				//triangles << "v " << max_mesh.x << " " << max_mesh.y << " " << max_mesh.z << "\n";
				write_vec3(triangles, min_mesh);
				write_vec3(triangles, max_mesh);
			}
		}
		write_vec3(meshes, min_world);
		write_vec3(meshes, max_world);
		
	}
	



}