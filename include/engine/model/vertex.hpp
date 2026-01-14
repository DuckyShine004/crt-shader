#pragma once

#include <glm/glm.hpp>

namespace engine::model {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex() : Vertex(0.0f, 0.0f, 0.0f) {
    }

    Vertex(float x, float y, float z) : position(x, y, z), uv(0.0f, 0.0f) {
    }

    Vertex(float x, float y, float z, float u, float v) : position(x, y, z), uv(u, v) {
    }
};

} // namespace engine::model
