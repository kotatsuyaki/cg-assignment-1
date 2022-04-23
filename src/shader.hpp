#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glad/glad.h>

#include <memory>
#include <string_view>

class Window;

class Shader final {
  public:
    Shader(const Window& window, std::string_view vertex_shader_src,
           std::string_view fragment_shader_src);
    ~Shader();

    // Prevent copy, allow move
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;

    // Gets the location of a uniform variable.
    GLint uniform_location(std::string_view name);

  private:
    struct ShaderImpl;
    struct ShaderImplDeleter {
        void operator()(ShaderImpl*) const;
    };
    std::unique_ptr<ShaderImpl, ShaderImplDeleter> impl;
};

#endif
