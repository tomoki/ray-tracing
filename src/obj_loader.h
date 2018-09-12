#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct face {
    vec3 vertices;
};

struct model {
    std::vector<vec3> vertices;
    std::vector<vec3> normals; // per vertex
    std::vector<face> faces;
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

bool load_obj(const std::string& path, model& model)
{
    std::fstream fs(path);
    if (!fs) {
        return false;
    }
    std::string s;
    while(getline(fs, s)) {
        // empty line?
        if (s.length() == 0)
            continue;
        // comment
        if (s[0] == '#')
            continue;

        auto v = split(s, ' ');
        if (v[0] == "v") {
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
            model.faces.push_back(f);
        } else {
            std::cerr << "Unknown: " << s << std::endl;
        }
    }
    return true;
}
