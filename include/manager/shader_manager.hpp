#pragma once

#include <map>
#include <memory>

#include "manager/manager.hpp"

#include "engine/shader/shader.hpp"

using namespace engine::shader;

namespace manager {

class ShaderManager final : public Manager {
  public:
    static ShaderManager &get_instance();

    Shader &get_shader(const std::string &name);

    void initialise();

  private:
    static inline constexpr const char *_DIRECTORY = "resources/shaders/";

    static inline constexpr const char *_VERTEX_SHADER_EXTENSION = ".vert";
    static inline constexpr const char *_FRAGMENT_SHADER_EXTENSION = ".frag";

    ShaderManager();

    ~ShaderManager();

    void add_shader(const std::string &path);

    std::map<std::string, std::unique_ptr<Shader>> _shaders;
};

}; // namespace manager
