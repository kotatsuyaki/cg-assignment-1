#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <memory>
#include <string_view>

#include "matrix.hpp"

class Window;

// Wrapper class for compiled OpenGL shader program objects.
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

    // Sets uniform with name to value of mat.
    // The input matrix should be stored row-major.
    void set_uniform(std::string_view name, const Matrix4& mat);

    // Sets uniform with name to value of vec.
    // The input matrix should be stored row-major.
    void set_uniform(std::string_view name, const Vector3& vec);

  private:
    class Impl;
    struct ImplDeleter {
        void operator()(Impl*) const;
    };
    std::unique_ptr<Impl, ImplDeleter> impl;
};

#endif
