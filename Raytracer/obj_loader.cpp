#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include "obj_loader.h"
#include "glm/glm.hpp"
#include "utils.h"
#include <iostream>

void ReadObj(const char* filename, std::vector<Tris>& tri_data) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, filename, nullptr, true);

    if (!ret) {
        throw std::runtime_error("Failed to load OBJ: " + warn + err);
    }

    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            if (fv != 3) {
                std::cerr << "Non-triangular face detected, skipping.\n";
                index_offset += fv;
                continue;
            }

            glm::vec3 verts[3];
            glm::vec3 normals[3];
            bool has_normals = true;

            for (int v = 0; v < 3; ++v) {
                const auto idx = shape.mesh.indices[index_offset + v];

                // Vertex position
                float x = attrib.vertices[3 * idx.vertex_index + 0];
                float y = attrib.vertices[3 * idx.vertex_index + 1];
                float z = attrib.vertices[3 * idx.vertex_index + 2];
                verts[v] = glm::vec3(x, y, z);

                // Check if normal index is valid
                if (idx.normal_index >= 0 && attrib.normals.size() >= 3 * (idx.normal_index + 1)) {
                    float nx = attrib.normals[3 * idx.normal_index + 0];
                    float ny = attrib.normals[3 * idx.normal_index + 1];
                    float nz = attrib.normals[3 * idx.normal_index + 2];
                    normals[v] = glm::vec3(nx, ny, nz);
                }
                else {
                    has_normals = false;
                }
            }

            glm::vec3 face_normal;
            if (has_normals) {
                // Average vertex normals for face normal
                face_normal = glm::normalize(normals[0] + normals[1] + normals[2]);
            }
            else {
                // Compute manually
                glm::vec3 e1 = verts[1] - verts[0];
                glm::vec3 e2 = verts[2] - verts[0];
                face_normal = glm::normalize(glm::cross(e1, e2));
            }

            tri_data.push_back(Tris(verts[0], verts[1], verts[2], face_normal));
            index_offset += 3;
        }
    }
}

