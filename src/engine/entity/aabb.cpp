#include "engine/entity/aabb.hpp"

#include "common/constant.hpp"

using namespace engine::shader;

using namespace common;

namespace engine::entity {

AABB::AABB(float x, float y, float z, float size) {
    this->_min_x = x;
    this->_min_y = y;
    this->_min_z = z;

    this->_max_x = x + size;
    this->_max_y = y + size;
    this->_max_z = z + size;

    /* Bottom */
    this->_mesh.add_vertex(this->_min_x, this->_min_y, this->_min_z);
    this->_mesh.add_vertex(this->_min_x, this->_min_y, this->_max_z);
    this->_mesh.add_vertex(this->_max_x, this->_min_y, this->_max_z);
    this->_mesh.add_vertex(this->_max_x, this->_min_y, this->_min_z);

    /* Top */
    this->_mesh.add_vertex(this->_min_x, this->_max_y, this->_min_z);
    this->_mesh.add_vertex(this->_min_x, this->_max_y, this->_max_z);
    this->_mesh.add_vertex(this->_max_x, this->_max_y, this->_max_z);
    this->_mesh.add_vertex(this->_max_x, this->_max_y, this->_min_z);

    this->_mesh.add_indices(this->_INDICES);

    this->_mesh.upload();
}

void AABB::render(Shader &shader) {
    glm::mat4 model(1.0f);

    shader.set_matrix4fv("u_model", model);

    shader.set_vector3f("u_colour", WHITE_RGB);

    this->_mesh.render(this->_TOPOLOGY);
}

bool AABB::collide(Sphere &sphere) {
    glm::vec3 position = sphere.get_position();

    float radius = sphere.get_radius();

    float x = glm::clamp(position.x, this->_min_x, this->_max_x);
    float y = glm::clamp(position.y, this->_min_y, this->_max_y);
    float z = glm::clamp(position.z, this->_min_z, this->_max_z);

    glm::vec3 delta = position - glm::vec3(x, y, z);

    return glm::dot(delta, delta) <= radius * radius;
}

} // namespace engine::entity
