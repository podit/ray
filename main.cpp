#include <float.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

vec3 colour(const ray& r, hitable *world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.001, DBL_MAX, rec)) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation*colour(scattered, world, depth+1);
    }
    else {
      return vec3(0,0,0);
    }
  }
  else {
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}

hitable *random_scene() {
  int n = 500;
  hitable **list = new hitable*[n+1];
  list[0] = new sphere(vec3(0,-1000,0),1000, 
      std::make_shared<lambertian>(vec3(0.5,0.5,0.5)));
  int i = 1;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      double choose_mat = drand48();
      vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
      if ((center-vec3(4,0.2,0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          list[i++] = new sphere(center, 0.2, 
              std::make_shared<lambertian>(vec3(drand48()*drand48(), 
                  drand48()*drand48(), drand48()*drand48())));
        }
        else if (choose_mat < 0.95) {
          list[i++] = new sphere(center, 0.2, 
              std::make_shared<metal>(vec3(0.5*(1 + drand48()), 
                  0.5*(1 + drand48()), 0.5*(1 + drand48())), 
                0.5*drand48()));
        }
        else {
          list[i++] = new sphere(center, 0.2, 
              std::make_shared<dielectric>(1.5));
        }
      }
    }
  }

  list[i++] = new sphere(vec3(4,1,0), 1.0, 
      std::make_shared<dielectric>(1.5));
  list[i++] = new sphere(vec3(-2,1,0), 1.0, 
      std::make_shared<lambertian>(vec3(0.4,0.2,0.1)));
  list[i++] = new sphere(vec3(0,1,0), 1.0, 
      std::make_shared<metal>(vec3(0.7,0.6,0.5), 0.0));

  return new hitable_list(list,i);
}

int main() {
  int nx = 640;
  int ny = 360;
  int ns = 3;
  char data [3*nx*ny];
  int imgPos = 0;

  hitable *world = random_scene();

  vec3 lookfrom(13,5,3);
  vec3 lookat(0,0,0);
  double dist_to_focus = 10.0; //(lookfrom-lookat).length();
  double aperture = 0.1;
  camera cam(lookfrom, lookat, vec3(0,1,0), 20, 
      double(nx)/double(ny), aperture, dist_to_focus);

  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);
      for (int s=0; s < ns; s++) {

        double u = double(i + drand48()) / double(nx);
        double v = double(j + drand48()) / double(ny);

        ray r = cam.get_ray(u, v);

        col += colour(r, world,0);
      }
      col /= double(ns);
      col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );

      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);

      data[imgPos++] = ir;
      data[imgPos++] = ig;
      data[imgPos++] = ib;
    }
  }
  stbi_write_png("out.png", nx, ny, 3, data, nx*3);
}
