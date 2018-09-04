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
#include "rect.h"

#include <climits>
#include <chrono>
#include <thread>
#include <queue>
#include <iostream>

vec3 color(const ray& r, hitable *world, int depth=0)
{
    hit_record rec;
    if (world->hit(r, 0.001, 1e9, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * color(scattered, world, depth+1);
        else
            return emitted;
    } else
        return vec3(0, 0, 0);
}

hitable *random_scene()
{
    int n = 500;
    hitable **list = new hitable*[n+1];
    int i = 0;
    for(int a=-11; a < 11; a++) {
        for(int b=-11; b<11; b++) {
            float choose_mat = rand_float();
            vec3 center(a + 0.9 * rand_float(), 0.2, b + 0.9 * rand_float());
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
    hitable** ret = new hitable*[10];
    int ret_i = 0;
    ret[ret_i++] = new bvh_node(list, i, 0, 1);
    ret[ret_i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)))));
    ret[ret_i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    ret[ret_i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    ret[ret_i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    ret[ret_i++] = new xy_rect(-2, 1, 2, 3, -3, new diffuse_light(new constant_texture(vec3(6.0, 6.0, 6.0))));
    return new hitable_list(ret, ret_i);
}

hitable* two_perlin_spheres()
{
    texture* pertext = new noise_texture();
    hitable** list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hitable_list(list, 2);
}

// hitable* texture_scene()
// {
//     int nx, ny, nn;
//     unsigned char* tex_data = stbi_load("tex.jpg", &nx, &ny, &nn, 0);
//     material* mat = new lambertian(new image_texture(tex_data, nx, ny));
//     return new sphere(vec3(0, 2, 0), 2, mat);
// }

int main(int argc, char** argv)
{
    int nx = 3840;
    int ny = 2160;
    int ns = 100;

    // For show performance
    bool show_performance = true;
    int total_ray = nx * ny * ns;
    int processed_ray = 0;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point tmp_time = std::chrono::system_clock::now();

    std::cout << "P3\n"
              << nx << " " << ny << "\n"
              << 255 << "\n";

    hitable *world = random_scene();
    vec3 lookfrom(12, 2, 3);
    vec3 lookat(0, 0.5, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);

    // hitable* world = two_perlin_spheres();
    // // hitable* world = texture_scene();
    // vec3 lookfrom(13, 2, 3);
    // vec3 lookat(0, 0, 0);
    // float dist_to_focus = 10.0;
    // float aperture = 0.0;
    // camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);

    std::vector<std::vector<vec3>> colors(ny, std::vector<vec3>(nx));

    std::vector<std::thread> threads;
    int number_of_threads = std::thread::hardware_concurrency();
    std::vector<std::queue<int>> y_per_threads(number_of_threads);
    std::vector<int> done_task_per_threads(number_of_threads);
    std::vector<int> tasks_per_threads(number_of_threads);
    {
        int k = 0;
        for (int j = ny - 1; j >= 0; j--) {
            y_per_threads[k].push(j);
            k = (k + 1) % number_of_threads;
        }
        for (int i = 0; i < tasks_per_threads.size(); i++) {
            tasks_per_threads[i] = y_per_threads[i].size();
        }
        for (int k = 0; k < y_per_threads.size(); k++) {
            threads.push_back(std::thread([k, &y_per_threads, &done_task_per_threads, &colors, nx, ny, ns, cam, world]() {
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
                    done_task_per_threads[k]++;
                }
            }));
        }
    }
    if (show_performance)
        threads.push_back(std::thread([number_of_threads, &done_task_per_threads, &tasks_per_threads]() {
            bool done = false;
            while (!done) {
                bool still_in_progress = false;
                for (int i = 0; i < number_of_threads; i++) {
                    still_in_progress |= done_task_per_threads[i] != tasks_per_threads[i];
                    std::cerr << i << " " << done_task_per_threads[i] << "/" << tasks_per_threads[i] << std::endl;
                }
                if (!still_in_progress)
                    done = true;
                else {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    for (int i = 0; i < number_of_threads; i++) {
                        std::cerr << "\x1b[1A";
                    }
                }

            }
        }));
    for (auto& t : threads)
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

     if (show_performance) {
        std::chrono::system_clock::time_point end  = std::chrono::system_clock::now();
        std::chrono::system_clock::duration dur = end - start;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        std::cerr << std::endl;
        std::cerr << "Total: " << ms << " ms" << std::endl;
        std::cerr << "Ray:   " << nx * ny * ns / (ms / 1000.0) << " ray/s" << std::endl;
        std::cerr << "Pixel: " << nx * ny / (ms / 1000.0) << " pixel/s" << std::endl;
     }
     return 0;
}
