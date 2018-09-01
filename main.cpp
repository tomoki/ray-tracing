#include "bvh.h"
#include "camera.h"
#include "common.h"
#include "vec3.h"
#include "ray.h"
#include "texture.h"
#include "hitable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"

#include <climits>
#include <iostream>
#include <thread>
#include <queue>

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

hitable *random_scene()
{
    int n = 500;
    hitable **list = new hitable*[n+1];
    hitable** ret = new hitable*[10];
    ret[1] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)))));
    ret[2] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    ret[3] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    ret[4] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    std::vector<std::pair<vec3, float>> ss;
    ss.push_back(std::make_pair(vec3(0 , 1, 0), 1.0));
    ss.push_back(std::make_pair(vec3(-4, 1, 0), 1.0));
    ss.push_back(std::make_pair(vec3(4 , 1, 0), 1.0));

    auto collision = [&ss](const std::pair<vec3, float>& p) {
        for(const auto& i : ss) {
            if ((p.first - i.first).length() < i.second + p.second + 0.1)
                return true;
        }
        return false;
    };

    int i = 0;
    for(int a=-11; a < 11; a++) {
        for(int b=-11; b<11; b++) {
            float choose_mat = rand_float();
            vec3 center;
            int k = 0;
            do {
                center = vec3(a + 0.9 * rand_float(), 0.2, b + 0.9 * rand_float());
                k++;
            } while (collision(std::make_pair(center, 0.2)) && k < 10);
            if (k == 10)
                continue;
            ss.push_back(std::make_pair(center, 0.2));

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8)
                    list[i++] = new moving_sphere(
                                    center,
                                    center + vec3(0, 0.5 * rand_float(), 0),
                                    0, 1, 0.2,
                                    new lambertian(new constant_texture(vec3(
                                        rand_float() * rand_float(),
                                        rand_float() * rand_float(),
                                        rand_float() * rand_float()))));
                else if(choose_mat < 0.95)
                    list[i++] = new sphere(center, 0.2,
                                    new metal(vec3(
                                        0.5 * (1 + rand_float()),
                                        0.5 * (1 + rand_float()),
                                        0.5 * (1 + rand_float())), 0.5 * rand_float()));
                else
                    list[i++] = new sphere(center, 0.2,
                                    new dielectric(0.2));
            } else {
                list[i++] = new sphere(center, 0.2, new dielectric(1.5));
            }
        }
    }
    ret[0] = new bvh_node(list, i, 0, 1);

    return new hitable_list(ret, 5);
}

int main(int argc, char** argv)
{
    int nx = 7680;
    int ny = 4320;
    int ns = 1000;

    std::cout << "P3\n"
              << nx << " " << ny << "\n"
              << 255 << "\n";

    hitable *world = random_scene();

    vec3 lookfrom(12, 2, 3);
    vec3 lookat(0, 0.5, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);


    std::vector<std::vector<vec3>> colors(ny, std::vector<vec3>(nx));

    std::vector<std::thread> threads;
    std::vector<std::queue<int>> y_per_threads(std::thread::hardware_concurrency());
    {
        int k = 0;
        for (int j = ny - 1; j >= 0; j--) {
            y_per_threads[k].push(j);
            k = (k + 1) % std::thread::hardware_concurrency();
        }
        for (int k = 0; k < y_per_threads.size(); k++) {
            threads.push_back(std::thread([k, &y_per_threads, &colors, nx, ny, ns, cam, world]() {
                auto& q = y_per_threads[k];
                while(!q.empty()) {
                    int j = q.front();
                    q.pop();
                    for (int i = 0; i < nx; i++) {
                        vec3 total_col(0, 0, 0);
                        for (int k = 0; k < ns; k++) {
                            float u = 1.0 * (i + rand_float() - 0.5) / nx;
                            float v = 1.0 * (j + rand_float() - 0.5) / ny;
                            ray r = cam.get_ray(u, v);
                            vec3 p = r.point_at_parameter(2.0); // ????
                            total_col += color(r, world);
                        }
                        total_col /= float(ns);
                        // gamma ほせい
                        total_col = vec3(sqrt(total_col[0]), sqrt(total_col[1]), sqrt(total_col[2]));
                        colors[j][i] = total_col;
                    }
                }
            }));
        }
    }
    for(auto& t : threads)
        t.join();

    for (int j = ny - 1; j >= 0; j--) {
       for (int i = 0; i < nx; i++) {
           vec3 total_col = colors[j][i];
           int ir = (int)(255 * total_col.r());
           int ig = (int)(255 * total_col.g());
           int ib = (int)(255 * total_col.b());
           std::cout << ir << " " << ig << " " << ib << "\n";
       }
    }
    return 0;
}
