#include "engine/engine.hpp"

#include "engine/shader/shader.hpp"

#include "manager/shader_manager.hpp"
#include "manager/display_manager.hpp"

#include "logger/logger_macros.hpp"

using namespace engine::entity;

using namespace manager;

namespace engine {

void Engine::initialise() {
    this->_camera.update_projection();

    this->_quads.emplace_back(0.0f, -1.0f, 0.0f, 3.0f, 3.0f, -90.0f, 0.0f, 0.0f);
    this->_spheres.emplace_back();

    for (Quad &quad : this->_quads) {
        quad.get_mesh().upload();
    }

    this->_crt.get_mesh().upload();

    // WARN: scene post processing here
    glGenFramebuffers(1, &this->_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);

    DisplayManager &display_manager = DisplayManager::get_instance();

    int display_width = display_manager.get_width();
    int display_height = display_manager.get_height();

    glGenTextures(1, &this->_texture);
    glBindTexture(GL_TEXTURE_2D, this->_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, display_width, display_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_texture, 0);

    glGenRenderbuffers(1, &this->_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_width, display_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->_rbo);

    // WARN: shadow pre processing here
    glGenFramebuffers(1, &this->_shadow_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_shadow_fbo);

    glGenTextures(1, &this->_shadow_texture);
    glBindTexture(GL_TEXTURE_2D, this->_shadow_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->_shadow_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void Engine::update(GLFWwindow *window, float delta_time) {
    this->_camera.update(window, delta_time);
}

// Generate shadow map -> Render scene as texture with lighting + shadow sampling -> Apply CRT filter
void Engine::render() {
    // Generate shadow map texture
    glBindFramebuffer(GL_FRAMEBUFFER, this->_shadow_fbo);
    glViewport(0, 0, 1024, 1024);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    Shader &shadow = ShaderManager::get_instance().get_shader("shadow");

    shadow.use();

    glm::vec3 light_direction = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
    glm::vec3 light_position = -light_direction * 10.0f;

    glm::mat4 light_projection = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
    glm::mat4 light_view = glm::lookAt(light_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_space = light_projection * light_view;

    shadow.set_matrix4fv("u_light_space", light_space);

    for (Sphere &sphere : _spheres) {
        sphere.render(shadow);
    }

    for (Quad &quad : _quads) {
        quad.render(shadow);
    }

    // Pre processing render texture
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fbo);

    DisplayManager &display_manager = DisplayManager::get_instance();

    int display_width = display_manager.get_width();
    int display_height = display_manager.get_height();

    if (display_manager.is_window_resized()) {

        LOG_DEBUG("Width: {}, Height: {}", display_width, display_height);

        glBindTexture(GL_TEXTURE_2D, this->_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, display_width, display_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_texture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, this->_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_width, display_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->_rbo);

        glViewport(0, 0, display_width, display_height);
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, display_width, display_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader &scene = ShaderManager::get_instance().get_shader("scene");

    scene.use();

    this->_camera.upload_model_view_projection(scene);
    this->_camera.upload_position(scene);

    // clang-format off
    glm::mat4 bias(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    );
    // clang-format on

    glm::mat4 shadow_bias = bias * light_space;

    scene.set_vector3f("u_light.direction", -0.2f, -1.0f, -0.3f);

    scene.set_vector3f("u_light.ambient", glm::vec3(0.1f));
    scene.set_vector3f("u_light.diffuse", glm::vec3(0.8f));
    scene.set_vector3f("u_light.specular", glm::vec3(1.0f));

    scene.set_matrix4fv("u_bias", shadow_bias);
    scene.set_matrix4fv("u_light_space", light_space);

    // Bind shadow texture
    scene.set_integer("u_shadow_map", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->_shadow_texture);

    for (Sphere &sphere : this->_spheres) {
        sphere.render(scene);
    }

    for (Quad &quad : this->_quads) {
        quad.render(scene);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply CRT post processing
    Shader &crt = ShaderManager::get_instance().get_shader("crt");

    crt.use();

    crt.set_integer("u_scene", 0);

    crt.set_float("u_brightness", 1.0f);
    crt.set_float("u_vignette_opacity", 1.0f);
    crt.set_float("u_vignette_roundness", 1.0f);

    crt.set_vector2f("u_curvature", 4.0f, 4.0f);
    crt.set_vector2f("u_resolution", static_cast<float>(display_width), static_cast<float>(display_height));
    crt.set_vector2f("u_scanline_opacity", 0.25f, 0.25f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_texture);

    this->_crt.render(crt);

    display_manager.set_is_window_resized(false);
}

camera::Camera &Engine::get_camera() {
    return this->_camera;
}

} // namespace engine
