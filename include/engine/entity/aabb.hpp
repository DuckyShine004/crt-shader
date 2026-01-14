#pragma once

#include "engine/entity/sphere.hpp"

#include "engine/model/mesh.hpp"
#include "engine/model/topology.hpp"

#include "engine/shader/shader.hpp"

namespace engine::entity {

class AABB {
  public:
    /**
     * @brief Construct AABB, bottom-to-top with anti-clockwise order, using (0,0,0) as origin anchor
     *
     * @param x: x position
     * @param y: y position
     * @param z: z position
     * @param size: size of AABB
     */
    AABB(float x, float y, float z, float size);

    void render(shader::Shader &shader);

    bool collide(engine::entity::Sphere &sphere);

  private:
    static inline constexpr engine::model::Topology _TOPOLOGY = engine::model::Topology::LINE;

    // clang-format off
    static inline constexpr GLuint _INDICES[24] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };
    // clang-format on

    engine::model::Mesh _mesh;

    float _min_x;
    float _min_y;
    float _min_z;

    float _max_x;
    float _max_y;
    float _max_z;
};

} // namespace engine::entity
