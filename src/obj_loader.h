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
    float dissolved            { 1.0 }; // d, Tr
    int illum                  { 0 }; // illum
};

struct face {
    vec3 vertices;
};

struct obj {
    std::string name;
    obj_material material;
    std::vector<face> faces;
};

struct model {
    std::vector<vec3> vertices;
    std::vector<vec3> normals; // per vertex
    std::vector<obj> objects;
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
        } else if(v[0] == "vn") {
            // normal
            vec3 normal(std::stof(v[1]), std::stof(v[2]), 0);
            if (v.size() == 4) // have "w"
                normal[2] = std::stof(v[3]);
            model.normals.push_back(normal);
        } else if(v[0] == "f") {
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
                // Vertex+UV+Normal | f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                // Vertex+Normal    | f v1//vn1 v2//vn2 v3//vn3
                auto sp = split(v[i], '/');
                // FIXME: handle only vertex for now.
                f.vertices[i - 1] = std::stoi(sp[0]) - 1;
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
            for (const auto& material : materials) {
                std::cerr << "-- " << material.name << " -- " << std::endl;
                std::cerr << " * ambient     = " << material.ambient << std::endl;
                std::cerr << " * diffuse     = " << material.diffuse << std::endl;
                std::cerr << " * specular    = " << material.specular << std::endl;
                std::cerr << " * specular(c) = " << material.specular_coefficient << std::endl;
                std::cerr << " * emissive(c) = " << material.emissive_coefficient << std::endl;
                std::cerr << " * shiness     = " << material.shiness << std::endl;
                std::cerr << " * illum       = " << material.illum << std::endl;
            }
        } else
            std::cerr << "Unknown: " << s << std::endl;
    }
    if (!(current_obj.name == "Default" && current_obj.faces.size() == 0)) {
        model.objects.push_back(current_obj);
    }
    return true;
}

