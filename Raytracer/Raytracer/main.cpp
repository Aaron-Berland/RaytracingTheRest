#include <iostream>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define CHANNEL_NUM 4
#include "sphere.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "global_random.h"
#include "material.h"
#include <chrono>
#include "bvh.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"

void cornell_box(hitable **scene, camera **cam, float aspect) {
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	*scene = new hitable_list(list, i);
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0),
		vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}


vec3 color(const ray& r, hitable *world, int depth) {
	hit_record rec;
	if(world->hit(r,0.001,FLT_MAX,rec)){
		ray scattered;
		vec3 albedo;
		float pdf;
		vec3 emittedColor = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
	
		if(depth < 50 && rec.mat_ptr->scatter(r, rec, albedo, scattered,pdf)) {
			return emittedColor + albedo*rec.mat_ptr->scattering_pdf(r,rec,scattered)*color(scattered, world, depth + 1)/pdf;
		}
		else {
			return emittedColor;
		}
	}
	else {
		/*TODO: Add Enviroment Light Classes
					- Add Procedural Sky or Environment Mapp
					- Black
					- Color gradient
					- Sphere/Cube Mapping
		*/
		/* Simple Color Gradiente for SkyColor
		vec3 unit_direction = unit_vector(r.direction());
		float t = .5*(unit_direction.y() + 1.0);
		//std::cout << t << " El float \n";
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);*/
		return vec3(0.0, 0.0, 0.0);
	}
}
int main() {
	/* Code for random numbers between 0 -1  */
	
	// initialize the random number generator with time-dependent seed
	initializeRandom();
	int width = 500;
	int height = 500;
	int num_rays = 50;
	hitable *world;
	camera *cam;
	float aspect = float(height) / float(width);
	cornell_box(&world, &cam, aspect);
	unsigned char* output_image;
	output_image = (unsigned char*) malloc(width * height * CHANNEL_NUM* sizeof(unsigned char));
	int tenPercent = 0;
	int cantTenPercent = width*height / 10;
	int j = 0;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for (int i = 0; i < width * height; i++) {

		vec3 col(0.0, 0.0, 0.0);
		for (int j = 0; j < num_rays; j++) {
			float u = float((i%width) + get_random()) / float(width);
			float v = 1 - float((i / width) + get_random())/ float(height);
			ray r = cam->get_ray(u, v);
			col += color(r, world,0);
		}
		col /= float(num_rays);
		col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
		//std::cout << "Direction : " << r.direction() << std::endl;
		//std::cout << "Direction Normalized: " << unit_vector(r.direction()) << std::endl;
		//std::cout <<"Color : " << col << std::endl;
		//std::cout << int(255.99*col[0]) << " " << 255-int(255.99*col[1])<< " " << int(255.99*col[2]) << std::endl;
		output_image[j++] = int(255.99*col[0]);
		output_image[j++] = int(255.99*col[1]);
		output_image[j++] = int(255.99*col[2]);
		/*
		output_image[j++] = 255;
		output_image[j++] = 0;
		output_image[j++] = 0;
		*/
		output_image[j++] = 255;

		if (i%cantTenPercent == 0) {
			std::cout << "Progress = " << tenPercent * 10 << "." << std::endl;
			tenPercent++;
		}

	}
	stbi_write_png("Book2-Cover.png", width, height, CHANNEL_NUM, output_image, width*CHANNEL_NUM);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << std::endl;
	return 0;
}