#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <memory>
#include <string_view>

#include <glad/glad.h>

#include "matrix.hpp"

class Window;

// Wrapper class for compiled OpenGL shader program objects.
class Shader final {
  public:
    Shader(const Window& window, std::string_view vertex_shader_src,
           std::string_view fragment_shader_src);
    ~Shader();

    // The input matrix should be stored row-major.
    void set_uniform(std::string_view name, const Matrix4& mat);
    void set_uniform(std::string_view name, const Vector3& vec);
    void set_uniform(std::string_view name, GLint value);
    void set_uniform(std::string_view name, GLfloat value);

    GLint mag_filter() const;
    GLint min_filter() const;

    void toggle_mag_filter();
    void toggle_min_filter();

  private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

#endif
