#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "engine/shader/shader.hpp"

#include "engine/camera/direction.hpp"

namespace engine::camera {

struct RotationComponent {
    float yaw;
    float pitch;

    glm::vec3 direction;

    RotationComponent() : yaw(0.0f), pitch(0.0f), direction(glm::vec3(0.0f)) {
    }
};

struct ViewComponent {
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 front;

    ViewComponent() : right(glm::vec3(0.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
    }
};

struct MVPComponent {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    MVPComponent() : model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), projection(glm::mat4(1.0f)) {
    }
};

class Camera {
  public:
    Camera();

    Camera(glm::vec3 position);

    Camera(float x, float y, float z);

    void update(GLFWwindow *window, float delta_time);

    void update_view();

    void update_projection();

    void upload_view_projection(engine::shader::Shader &shader);

    void upload_model_view_projection(engine::shader::Shader &shader);

    void move(Direction direction, float delta_time);

    void rotate(double x, double y);

    void scroll(double x, double y);

    glm::vec3 get_front();

    glm::vec3 position;

  private:
    static inline constexpr float _FIELD_OF_VIEW = 45.0f;

    static inline constexpr std::pair<float, float> _FIELD_OF_VIEW_LIMITS = {1.0f, 90.0f};

    static inline constexpr float _NEAR = 0.1f;
    static inline constexpr float _FAR = 100.0f;

    static inline constexpr float _YAW = 90.0f;
    static inline constexpr float _PITCH = 0.0f;
    static inline constexpr float _PITCH_LIMIT = 89.0f;
    static inline constexpr float _SENSITIVITY = 0.025f;

    static inline constexpr float _SPEED = 10.0f;

    MVPComponent _MVP_component;

    ViewComponent _view_component;

    RotationComponent _rotation_component;

    float _field_of_view;

    glm::vec2 _last_cursor_position;

    glm::vec2 get_cursor_offset(glm::vec2 cursor_position);

    void update_rotation_component(glm::vec2 offset);

    void update_view_component();
};

} // namespace engine::camera
