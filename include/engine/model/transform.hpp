#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace engine::model {

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;

    Transform() : Transform(glm::vec3(0.0f), glm::vec3(1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {
    }

    Transform(glm::vec3 position, glm::vec3 scale) : Transform(position, scale, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {
    }

    Transform(glm::vec3 position, glm::vec3 scale, glm::quat rotation) : position(position), scale(scale), rotation(rotation) {
    }

    void set_rotation_euler_x(float angle) {
        // rotation.xglm::radians(angle);
    }

    void set_rotation_euler_y(float angle) {
    }

    void set_rotation_euler_z(float angle) {
    }
};

} // namespace engine::model
