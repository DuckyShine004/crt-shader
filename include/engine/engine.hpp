#pragma once

#include "engine/camera/camera.hpp"

#include "engine/entity/quad.hpp"
#include "engine/entity/sphere.hpp"

namespace engine {

class Engine {
  public:
    void initialise();

    void update(GLFWwindow *window, float delta_time);

    void render();

    camera::Camera &get_camera();

  private:
    static inline constexpr int _NUMBER_OF_SPHERES = 10;

    camera::Camera _camera;

    std::vector<engine::entity::Sphere> _spheres;

    std::vector<engine::entity::Quad> _quads;

    GLuint _fbo;
    GLuint _texture;
    GLuint _rbo;

    engine::entity::Quad _crt;
};

} // namespace engine
