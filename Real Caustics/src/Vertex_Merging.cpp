#include "Vertex_Merging.h"
#include "Scene.h"
#include "vec3.h"
#include "ray.h"
#include "BXDF.h"
#include "Path_Vertices.h"
#include "Camera.h"
#include "MLT_Sampler.h"
#include <cmath>
#include <fstream>

extern const float PI;
extern const float inf;
extern Scene* scene;



void Generate_light_sample(Light_Sub_Path_State& light_state, MLT_Sampler& Sampler)
{
	scene->lights.sample_lights(light_state, Sampler);
	light_state.path_length = 0;
	light_state.diffuse_interactions_count = 0;
	light_state.last_interaction = BxDFType::BSDF_DIFFUSE;
	light_state.dVM = 0.0f;
}

vec2 Generate_camera_sample(int pixel_index, Camera_Sub_Path_State& camera_state, const Camera& camera)
{
	const int resX = camera.pixel_width;
	const int resY = camera.pixel_height;

	const float x = pixel_index % resX;
	const float y = pixel_index / resX;

	// Jitter pixel
	vec2 sample = vec2(x + random_float(0.0f, 1.0f), y + random_float(0.0f, 1.0f));

	ray r = camera.get_ray(sample.x, sample.y);
	camera_state.origin = r.origin;
	camera_state.direction = r.direction;
	camera_state.path_length = 0;
	camera_state.throughput = 1.0f;

	camera_state.dVM = 0.0f;
	return sample;
}

bool Sample_Scattering_light(BxDF* brdf, const Isect& hit_point, Light_Sub_Path_State& light_state, MLT_Sampler& Sampler, bool vertex_was_stored)
{
	float forward_pdf, reverse_pdf, cos_theta_out, continuation_prob;
	vec3 new_direction;
	BxDFType sampled_event;

	vec3 brdf_factor = brdf->sample(new_direction, hit_point.geometric_normal, forward_pdf, reverse_pdf, cos_theta_out, continuation_prob, sampled_event, Sampler);

	if (brdf_factor.x == 0.0f && brdf_factor.y == 0.0f && brdf_factor.z == 0.0f)
		return false;

	
	if (light_state.path_length > 2)
	{
		if (random_float_0_1() > continuation_prob)
		{
			return false;
		}
		brdf_factor /= continuation_prob;
	}
	
	light_state.origin = hit_point.position;
	light_state.direction = new_direction;
	if (sampled_event & BxDFType::BSDF_DIFFUSE)
		light_state.diffuse_interactions_count++;

	light_state.throughput *= brdf_factor; //* correct_shading_normal(-hit_point.direction, new_direction, hit_point);
	light_state.last_interaction = sampled_event;

	//if (brdf_factor.x < 0 || brdf_factor.y < 0 || brdf_factor.z < 0)
	//{
	//	std::cout << "BAD IN BRDF_FACTOR";
	//}
	if (!vertex_was_stored)
	{
		// brdf.local_dir.z  - cos theta view vector with normal
		light_state.dVM *= (cos_theta_out / forward_pdf) * reverse_pdf;
	}
	else
	{
		light_state.dVM = (cos_theta_out / forward_pdf) * (1.0f + reverse_pdf * light_state.dVM);
	}
	//if (light_state.dVM < 0)
	//{
	//	std::cout << light_state.dVM << " ";
	//	std::cout << forward_pdf << "\n";
	//}
	return true;
}

bool Sample_Scattering_camera(BxDF* brdf, const Isect& hit_point, Camera_Sub_Path_State& camera_state, MLT_Sampler& Sampler)
{
	float forward_pdf;
	float reverse_pdf;
	float cos_theta_out;
	float continuation_prob;
	vec3 new_direction;
	BxDFType sampled_event;
	
	vec3 brdf_factor = brdf->sample(new_direction, hit_point.geometric_normal, forward_pdf, reverse_pdf, cos_theta_out, continuation_prob, sampled_event, Sampler);

	if (brdf_factor.x == 0.0f && brdf_factor.y == 0.0f && brdf_factor.z == 0.0f)
		return false;

	if (camera_state.path_length > 2)
	{
		if (random_float_0_1() > continuation_prob)
		{
			return false;
		}
		brdf_factor /= continuation_prob;
	}
	camera_state.origin = hit_point.position;
	camera_state.direction = new_direction;
	camera_state.throughput *= brdf_factor;

	if (sampled_event & BSDF_SPECULAR)
	{
		// brdf.local_dir.z  - cos theta view vector with normal
		camera_state.dVM *= (cos_theta_out / forward_pdf) * reverse_pdf;
	}
	else
	{
		camera_state.dVM = (cos_theta_out / forward_pdf) * (1.0f + reverse_pdf * camera_state.dVM);
	}
	
	return true;
}




void Vertex_Merging::trace_light_path(bool& visible, Path_Vertices& path_vertices, MLT_Sampler& Sampler, int index, Debuggg_Stuff& debug)
{
	Light_Sub_Path_State light_state;

	Generate_light_sample(light_state, Sampler);
	debug.light_dir = light_state.direction;

	for (;; ++light_state.path_length)
	{
		ray r = ray(light_state.origin + light_state.direction * 0.001f, light_state.direction);

		Isect hit_inf;

		if (!scene->hit(r, 0.001f, inf, hit_inf, 0))
			break;

		BxDF* brdf = nullptr;
		
		//MaterialInfo MatInf = hit_inf.compute_scattering_functions(scene->materials);
		hit_inf.compute_scattering_functions(&brdf, scene->materials, TransportMode::Importance);


		if (!brdf->IsValid())
			break;

		{
			// divide by cos_theta - not down in sample scattering
			light_state.dVM /= fabs(brdf->cos_theta_in());

		}

		bool vertex_is_stored = false;
		if (!brdf->Is_delta() && light_state.last_interaction & BSDF_GLOSSY)
		{
			Path_Vertex light_vertex;
			light_vertex.position = hit_inf.position;
			light_vertex.throughput = light_state.throughput;
			light_vertex.direction_world = -hit_inf.direction;
			light_vertex.dVM = light_state.dVM;
			visible = true;
			vertex_is_stored = true;
			path_vertices.add_light_vertex(light_vertex);
		}

		if (light_state.path_length > max_path_length || light_state.diffuse_interactions_count > 2)
			break;

		if (!Sample_Scattering_light(brdf, hit_inf, light_state, Sampler, vertex_is_stored))
		{
			delete brdf;
			break;
		}
		delete brdf;
	}
}

float Vertex_Merging::run_iteration(int iteration, float mut_size)
{
	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	
	const float light_paths_count = float(resX * resY);
	const int camera_path_count = resX * resY;

	float radius = base_radius;
	radius /= std::pow(float(iteration + 1), 0.5f * (1.0f - radius_alpha));
	radius = std::max(radius, 1e-7f);

	std::cout << "Radius:  " << radius << "\n";

	float radius_sqr = radius * radius;

	float VM_normalization = 1.0f / (PI * radius_sqr * light_paths_count);

	Path_Vertices path_vertices;

	path_vertices.init(light_paths_count);

	std::cout << "Tracing started" << std::endl;

	MLT_Sampler Sampler(max_path_length, 1.0f);

	int accepted = 1;
	int mutated = 0;

	int visible_uniform_paths = 0;
	/*
	std::ofstream uniform_path;
	uniform_path.open("D:\\Uniform_paths.obj");
	std::ofstream uniform_light_dir;
	uniform_light_dir.open("D:\\Uniform_light_sample.obj");*/
	for (int path_idx = 0; path_idx < light_paths_count; ++path_idx)
	{
		if (random_float_0_1() < 0.00001)
		{
			std::cout << (path_idx / light_paths_count) * 100.0f << "%";
		}
	
		Sampler.Init_Uniform();

		bool visible = false;
		Sampler.Start_Iteration();
		Debuggg_Stuff debug;
		trace_light_path(visible, path_vertices, Sampler, path_idx, debug);

		if (visible)
		{
			++visible_uniform_paths;
			/*
			vec3 p = path_vertices.light_vertices[path_vertices.light_vertices.size() - 1].position;
			uniform_path << "v " << p.x << " " << p.y << " " << p.z << "\n";
			vec3 light_dir_sample = debug.light_dir;
			uniform_light_dir << "v " << light_dir_sample.x << " " << light_dir_sample.y << " " << light_dir_sample.z << "\n";*/
		}
		else
		{
			//Mutate (Reject Uniform - large step)
			Sampler.Reject();
			Sampler.Mutate();
			mutated += 1;

			visible = false;
			Sampler.Start_Iteration();
			trace_light_path(visible, path_vertices, Sampler, path_idx, debug);
			if (visible)
				accepted += 1;
			else
				Sampler.Reject();

			float R = (float)accepted / (float)mutated;
			Sampler.mutation_size += (R - 0.234f) / (float)mutated;

			//if (random_float_0_1() < 0.00001 || path_idx == 0)
			//	std::cout << "Mutation Size:    " << Sampler.mutation_size << std::flush << "\r";
		}

	}
	int total_number_of_vertices = path_vertices.light_vertices.size();
	float visibility_normalization_constant = (float)visible_uniform_paths / (float)light_paths_count;
	for (int i = 0; i < total_number_of_vertices; ++i)
	{
		path_vertices.light_vertices[i].throughput *= visibility_normalization_constant;
	}

	std::cout << "100 %";
	std::cout << "\n";
	std::cout << "Tracing finished" << std::endl;

	std::cout << "Building KDTREE" << std::endl;
	path_vertices.build_kd_tree();

	std::cout << "KDTREE Finished" << std::endl;


	//std::ofstream o;
	//o.open("D:\\photon_locs.obj");
	//std::ofstream out_colors;
	//out_colors.open("D:\\colors.obj");
	/*
	for (int i = 0; i < path_vertices.light_vertices.size(); ++i)
	{
		o << "v " << path_vertices.light_vertices[i].position.x << " " << path_vertices.light_vertices[i].position.y << " " << path_vertices.light_vertices[i].position.z << "\n";
		//out_colors << "v " << path_vertices.light_vertices[i].throughput.x << " " << path_vertices.light_vertices[i].throughput.y << " " << path_vertices.light_vertices[i].throughput.z << "\n";
	}*/
	std::cout << "SUCCSEFUL PHOTON EMISSION:      " << path_vertices.light_vertices.size() / (float)light_paths_count << "\n";
	std::cout << "Camera Tracing started" << std::endl;

	
	
	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (random_float_0_1() < 0.00001)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%";
		}
			
		Camera_Sub_Path_State camera_state;

		const vec2 screen_sample = Generate_camera_sample(path_idx, camera_state, scene->camera);

		vec3 color(0.0f);
		Sampler.Init_Uniform();
		Sampler.Start_Iteration();

		for (;; ++camera_state.path_length)
		{
			ray r = ray(camera_state.origin + camera_state.direction * 0.001f, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.001f, inf, hit_inf, 0))
				break;

			BxDF* brdf = nullptr;

			hit_inf.compute_scattering_functions(&brdf, scene->materials, TransportMode::Radiance);
			if (!brdf->IsValid())
				break;
			

			{
				// divide by cos_theta - not down in sample scattering
				camera_state.dVM /= fabs(brdf->cos_theta_in());
			}

			if (!brdf->Is_delta())
			{
				color += camera_state.throughput * VM_normalization * path_vertices.get_contrib(hit_inf, radius_sqr, brdf, camera_state.dVM);
			}
			if (camera_state.path_length > max_path_length)
				break;

			if (!Sample_Scattering_camera(brdf, hit_inf, camera_state, Sampler))
			{
				delete brdf;
				break;
			}
			delete brdf;
		}

		
		image_buffer.add_color(screen_sample, color);
	}
	std::cout << "100 %";
	std::cout << "Camera Tracing finished" << std::endl;
	return Sampler.mutation_size;
}
void Vertex_Merging::run()
{
	image_buffer.init(scene->camera.pixel_width, scene->camera.pixel_height);

	float mut_size = 1.0f;
	for (int sample = 0; sample < samples; ++sample)
	{
		std::cout << "ITERATION:   " << sample << std::endl;
		mut_size = run_iteration(sample, mut_size);
	}

	image_buffer.scale(1.0f / (float)samples);
}



