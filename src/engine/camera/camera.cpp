#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera/camera.hpp"

#include "logger/logger_macros.hpp"

using namespace engine::shader;

namespace engine::camera {

Camera::Camera() : Camera(0.0f, 0.0f, 0.0f) {
}

Camera::Camera(float x, float y, float z) : position(x, y, z), _last_cursor_position(glm::vec2(-1.0f)), _field_of_view(this->_FIELD_OF_VIEW) {
}

void Camera::update_projection() {
    float aspect_ratio = 1280.0f / 720.0f;

    float field_of_view = glm::radians(this->_field_of_view);

    this->_MVP_component.projection = glm::perspective(field_of_view, aspect_ratio, this->_NEAR, this->_FAR);
}

void Camera::update_view() {
    this->_MVP_component.view = glm::lookAt(this->position, this->position + this->_view_component.front, this->_view_component.up);
}

void Camera::update(GLFWwindow *window, float delta_time) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->move(FORWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->move(BACKWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->move(RIGHT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->move(LEFT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        this->move(UP, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        this->move(DOWN, delta_time);
    }
}

void Camera::upload_view_projection(Shader &shader) {
    shader.set_matrix4fv("u_view", this->_MVP_component.view);
    shader.set_matrix4fv("u_projection", this->_MVP_component.projection);
}

void Camera::upload_model_view_projection(Shader &shader) {
    shader.set_matrix4fv("u_model", this->_MVP_component.model);
    shader.set_matrix4fv("u_view", this->_MVP_component.view);
    shader.set_matrix4fv("u_projection", this->_MVP_component.projection);
}

void Camera::move(Direction direction, float delta_time) {
    float velocity = this->_SPEED * delta_time;

    if (direction == FORWARD) {
        this->position += this->_view_component.front * velocity;
    }

    if (direction == BACKWARD) {
        this->position -= this->_view_component.front * velocity;
    }

    if (direction == LEFT) {
        this->position -= this->_view_component.right * velocity;
    }

    if (direction == RIGHT) {
        this->position += this->_view_component.right * velocity;
    }

    if (direction == UP) {
        this->position += this->_view_component.up * velocity;
    }

    if (direction == DOWN) {
        this->position -= this->_view_component.up * velocity;
    }

    this->update_view();
}

void Camera::rotate(double x, double y) {
    glm::vec2 cursor_position(x, y);

    if (this->_last_cursor_position.x == -1.0f && this->_last_cursor_position.y == -1.0f) {
        this->_last_cursor_position = cursor_position;
    }

    glm::vec2 offset = this->get_cursor_offset(cursor_position);

    this->_last_cursor_position = cursor_position;

    this->update_rotation_component(offset);
    this->update_view_component();
}

void Camera::scroll(double x, double y) {
    this->_field_of_view -= (float)y;
    this->_field_of_view = glm::clamp(this->_field_of_view, this->_FIELD_OF_VIEW_LIMITS.first, this->_FIELD_OF_VIEW_LIMITS.second);

    LOG_DEBUG("FOV: {}", this->_field_of_view);

    this->update_projection();
}

glm::vec2 Camera::get_cursor_offset(glm::vec2 cursor_position) {
    float offset_x = (cursor_position.x - this->_last_cursor_position.x) * this->_SENSITIVITY;
    float offset_y = (this->_last_cursor_position.y - cursor_position.y) * this->_SENSITIVITY;

    return glm::vec2(offset_x, offset_y);
}

void Camera::update_rotation_component(glm::vec2 offset) {
    this->_rotation_component.yaw += offset.x;
    this->_rotation_component.pitch = glm::clamp(this->_rotation_component.pitch + offset.y, -this->_PITCH_LIMIT, this->_PITCH_LIMIT);

    float theta = glm::radians(this->_rotation_component.yaw);
    float omega = glm::radians(this->_rotation_component.pitch);

    this->_rotation_component.direction.x = glm::cos(theta) * glm::cos(omega);
    this->_rotation_component.direction.y = glm::sin(omega);
    this->_rotation_component.direction.z = glm::sin(theta) * glm::cos(omega);
}

void Camera::update_view_component() {
    this->_view_component.front = glm::normalize(this->_rotation_component.direction);
    this->_view_component.right = glm::cross(this->_view_component.front, this->_view_component.up);

    this->update_view();
}

glm::vec3 Camera::get_front() {
    return this->_view_component.front;
}

} // namespace engine::camera
