#include "camera.h"
#include "common.h"
#include "vec3.h"
#include "ray.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

#include <climits>
#include <iostream>

vec3 color(const ray& r, hitable *world, int depth=0)
{
    hit_record rec;
    if (world->hit(r, 0.001, 1e9, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth+1);
        } else {
            return vec3(0, 0, 0);
        }
    }

    {
        // -1 ~ 1
        vec3 unit_direction = unit_vector(r.direction());
        // 0 - 1
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-t) * vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main(int argc, char** argv)
{
    int nx = 200;
    int ny = 100;
    int ns = 100;

    std::cout << "P3\n"
              << nx << " " << ny << "\n"
              << 255 << "\n";

    // hitable* list[2];
    // float R = cos(3.141592 / 4);
    // list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
    // list[1] = new sphere(vec3( R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
    // hitable *world = new hitable_list(list, 2);
    hitable* list[5];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.0)));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list, 5);

    camera cam(90, float(nx) / float(ny));
    for(int j=ny-1; j>=0; j--) {
        for(int i=0; i<nx; i++) {
            vec3 total_col(0, 0, 0);
            for(int k=0; k<ns; k++) {
                float u = 1.0 * (i + rand_float()-0.5) / nx;
                float v = 1.0 * (j + rand_float()-0.5) / ny;
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0); // ????
                total_col += color(r, world);
            }
            total_col /= float(ns);
            // gamma ほせい
            total_col = vec3(sqrt(total_col[0]), sqrt(total_col[1]), sqrt(total_col[2]));

            int ir = (int) (255 * total_col.r());
            int ig = (int) (255 * total_col.g());
            int ib = (int) (255 * total_col.b());

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    return 0;
}
