#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

struct obj_material {
    std::string name;
    vec3 ambient               { 1.0, 1.0, 1.0 }; // ka
    vec3 diffuse               { 1.0, 1.0, 1.0 }; // kd
    vec3 specular              { 1.0, 1.0, 1.0 }; // ks
    float specular_coefficient { 0 };             // ns
    vec3 emissive_coefficient  { 1.0, 1.0, 1.0 }; // ke
    float shiness              { 1.0 };           // ni
    float dissolved            { 1.0 };           // d, Tr
    int illum                  { 0 };             // illum
    std::vector<unsigned char> tex_color;         // Kd
    int tex_width { 0 };
    int tex_height { 0 };
};

struct face {
    std::vector<int> vertices_index { 0, 0, 0 };
    std::optional<std::vector<int>> tex_coords_index;
    std::optional<std::vector<int>> normals_index;
};

struct obj {
    std::string name;
    std::string material_name;
    std::vector<face> faces;
    bool smooth { false };
};

struct model {
    std::vector<vec3> vertices;
    std::vector<vec3> normals; // per vertex
    std::vector<vec3> tex_coords;
    std::vector<obj> objects;
    std::vector<obj_material> materials;
};

std::vector<std::string> split(std::string str, char delim)
{
    std::vector<std::string> res;
    size_t current = 0, found;
    while((found = str.find_first_of(delim, current)) != std::string::npos) {
        res.push_back(std::string(str, current, found - current));
        current = found + 1;
    }
    res.push_back(std::string(str, current, str.size() - current));
    return res;
}

bool load_mtl(const std::string& path, std::vector<obj_material>& materials)
{
    std::fstream fs(path);
    if (!fs)
        return false;

    std::string s;
    std::optional<obj_material> last_material;
    while (getline(fs, s)) {
        // Empty line
        if (s.length() == 0)
            continue;

        // Comment
        if (s[0] == '#')
            continue;

        auto v = split(s, ' ');

        if (v[0] == "newmtl") {
            if (last_material)
                materials.push_back(last_material.value());

            last_material = obj_material();
            last_material.value().name = v[1];
        } else if (v[0] == "Ns") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().specular_coefficient = std::stof(v[1]);
        } else if (v[0] == "Ni") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().shiness = std::stof(v[1]);
        } else if (v[0] == "d") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().dissolved = std::stof(v[1]);
        } else if (v[0] == "Tr") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().dissolved = (1 - std::stof(v[1]));
        } else if (v[0] == "Ni") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().shiness = std::stof(v[1]);
        } else if (v[0] == "illum") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().illum = std::stoi(v[1]);
        } else if (v[0] == "Ka") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 4) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().ambient = { std::stof(v[1]), std::stof(v[2]), std::stof(v[3]) } ;
        } else if (v[0] == "Kd") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 4) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().diffuse = { std::stof(v[1]), std::stof(v[2]), std::stof(v[3]) } ;
        } else if (v[0] == "Ks") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 4) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().specular = { std::stof(v[1]), std::stof(v[2]), std::stof(v[3]) } ;
        } else if (v[0] == "Ke") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 4) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            last_material.value().emissive_coefficient = { std::stof(v[1]), std::stof(v[2]), std::stof(v[3]) } ;
        } else if (v[0] == "map_Kd") {
            if (!last_material) {
                std::cerr << "newmtl not found" << std::endl;
                return false;
            }
            if (v.size() != 2) {
                std::cerr << "Format not supported: " << s << std::endl;
                return false;
            }
            int nx, ny, nn;
            unsigned char* tex_data = stbi_load(v[1].c_str(), &nx, &ny, &nn, STBI_rgb_alpha);
            if (!tex_data) {
                std::cerr << "Failed to open: " << v[1] << std::endl;
                continue;
            }
            std::vector<unsigned char> data(4 * nx * ny, 231);
            // for (int x = 0; x < nx; x++) {
            //     for (int y = 0; y < ny; y++) {
            //         data[4 * x + 4 * nx * y + 0] = tex_data[4 * x + 4 * nx * y + 0];
            //         data[4 * x + 4 * nx * y + 1] = tex_data[4 * x + 4 * nx * y + 1];
            //         data[4 * x + 4 * nx * y + 2] = tex_data[4 * x + 4 * nx * y + 2];
            //         data[4 * x + 4 * nx * y + 3] = 123;
            //     }
            // }
            // for (int i = 0; i < data.size(); i++) {
            //     std::cerr << int(data[i]) << " ";
            // }
            // std::cerr << std::endl;
            std::copy(tex_data, tex_data + 4 * nx * ny, data.begin());
            last_material.value().tex_color = data;
            last_material.value().tex_width = nx;
            last_material.value().tex_height = ny;
            free(tex_data);
        } else {
            std::cerr << "Unknown: " << s << std::endl;
        }
    }
    if (last_material)
        materials.push_back(last_material.value());

    return true;
}

bool load_obj(const std::string& path, model& model)
{
    std::fstream fs(path);
    if (!fs) {
        return false;
    }

    std::string s;
    obj current_obj; // should have default object
    current_obj.name = "Default";
    while (getline(fs, s)) {
        // empty line?
        if (s.length() == 0)
            continue;
        // comment
        if (s[0] == '#')
            continue;

        auto v = split(s, ' ');
        if (v[0] == "o") {
            if (current_obj.name == "Default" && current_obj.faces.size() == 0) {
                // We don't have default objects.
            } else {
                model.objects.push_back(current_obj);
                current_obj = obj();
            }
        } else if (v[0] == "v") {
            // vertex
            vec3 vert(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
            model.vertices.push_back(vert);
        } else if (v[0] == "vn") {
            // normal
            vec3 normal(std::stof(v[1]), std::stof(v[2]), 0);
            if (v.size() == 4) // have "w"
                normal[2] = std::stof(v[3]);
            model.normals.push_back(normal);
        } else if (v[0] == "vt") {
            vec3 tex_coord(std::stof(v[1]), std::stof(v[2]), 0);
            if (v.size() >= 4)
                tex_coord[3] = std::stof(v[3]);
            model.tex_coords.push_back(tex_coord);
        } else if (v[0] == "f") {
            // face
            if (v.size() != 4) {
                std::cerr << "Face has vertices more than 4: " << s << std::endl;
                return false;
            }
            face f;
            for (int i = 1; i < 4; i++) {
                // FIXME: format should have consistency.
                // Formats:
                // Vertex           | f v1 v2 v3
                // Vertex+UV        | f v1/vt1 v2/vt2 v3/vt3
                // Vertex+Normal    | f v1//vn1 v2//vn2 v3//vn3
                // Vertex+UV+Normal | f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                auto sp = split(v[i], '/');
                if (sp.size() == 1) {
                    // Vertex           | f v1 v2 v3
                    f.vertices_index[i - 1] = std::stoi(sp[0]) - 1;
                } else if(sp.size() == 2) {
                    // Vertex+UV        | f v1/vt1 v2/vt2 v3/vt3
                    f.vertices_index[i - 1] = std::stoi(sp[0]) - 1;
                    if (!f.tex_coords_index)
                        f.tex_coords_index = std::vector<int> { 0, 0, 0 };
                    f.tex_coords_index.value()[i - 1] = std::stoi(sp[1]) - 1;
                } else if (sp.size() == 3 && sp[1] == "") {
                    // Vertex+Normal    | f v1//vn1 v2//vn2 v3//vn3
                    f.vertices_index[i - 1] = std::stoi(sp[0]) - 1;
                    if (!f.normals_index)
                        f.normals_index = std::vector<int> { 0, 0, 0 };
                    f.normals_index.value()[i - 1] = std::stoi(sp[2]) - 1;
                } else if (sp.size() == 3) {
                    // Vertex+UV+Normal | f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                    f.vertices_index[i - 1] = std::stoi(sp[0]) - 1;
                    if (!f.tex_coords_index)
                        f.tex_coords_index = std::vector<int> { 0, 0, 0 };
                    f.tex_coords_index.value()[i - 1] = std::stoi(sp[1]) - 1;

                    if (!f.normals_index)
                        f.normals_index = std::vector<int> { 0, 0, 0 };
                    f.normals_index.value()[i - 1] = std::stoi(sp[2]) - 1;
                } else {
                    std::cerr << "Unknown format: " << s << std::endl;
                }
            }
            current_obj.faces.push_back(f);
        } else if (v[0] == "mtllib") {
            // mtl file locates same directory as obj file.
            std::string fname = v[1];
            std::filesystem::path material_path = std::filesystem::path(path).parent_path().concat(fname);
            std::vector<obj_material> materials;
            if (!load_mtl(material_path, materials)) {
                std::cerr << "Could not load " << material_path << std::endl;
                return false;
            }
            std::copy(materials.begin(), materials.end(), std::back_inserter(model.materials));
        } else if (v[0] == "usemtl") {
            std::string mtl_name = v[1];
            current_obj.material_name = v[1];
        } else if (v[0] == "s") {
            if (v[1] == "1" || v[0] == "on")
                current_obj.smooth = true;
            else if (v[1] == "0" || v[0] == "off")
                current_obj.smooth = false;
            else
                std::cerr << "Unknown option for s: " << v[1] << std::endl;
        } else
            std::cerr << "Unknown: " << s << std::endl;
    }
    if (!(current_obj.name == "Default" && current_obj.faces.size() == 0)) {
        model.objects.push_back(current_obj);
    }
    return true;
}

