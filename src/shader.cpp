#include "shader.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include "window.hpp"

class Shader::Impl {
  public:
    Impl(GLuint program) : program(program), uniform_locations() {}
    ~Impl() { glDeleteProgram(program); }

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    void set_uniform(std::string_view name, const Matrix4& mat);
    void set_uniform(std::string_view name, const Vector3& vec);

    GLint uniform_location(std::string_view name);

  private:
    GLuint program;

    // Internal cache for uniform locations
    std::unordered_map<std::string, GLint> uniform_locations;
};
void Shader::ImplDeleter::operator()(Impl* ptr) const { delete ptr; }

Shader::Shader(const Window& window, std::string_view vertex_shader_src,
               std::string_view fragment_shader_src) {
    window.make_current();

    GLuint v, f;

    const char* vs = vertex_shader_src.data();
    const char* fs = fragment_shader_src.data();

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v, 1, static_cast<const GLchar**>(&vs), nullptr);
    glShaderSource(f, 1, static_cast<const GLchar**>(&fs), nullptr);

    GLint success;
    GLchar info_log[1000];

    // Vertex shader
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(v, 1000, nullptr, info_log);
        throw std::runtime_error(std::string("Vertex shader compilation failed:\n") + info_log +
                                 "\n");
    }

    // Fragment shader
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(f, 1000, nullptr, info_log);
        throw std::runtime_error(std::string("Fragment shader compilation failed:\n") + info_log +
                                 "\n");
    }

    // Create program object
    GLuint p = glCreateProgram();
    if (p == 0) {
        throw std::runtime_error("Shader program creation failed");
    }

    // Attach shaders to the program object
    glAttachShader(p, f);
    glAttachShader(p, v);

    // Link program
    glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(p, 1000, nullptr, info_log);
        throw std::runtime_error(std::string("Shader program linking failed:\n") + info_log + "\n");
    }

    glDeleteShader(v);
    glDeleteShader(f);
    glUseProgram(p);

    impl = std::unique_ptr<Impl, ImplDeleter>(new Impl(p));
}
Shader::~Shader() {}

void Shader::set_uniform(std::string_view name, const Matrix4& mat) {
    impl->set_uniform(name, mat);
}
void Shader::Impl::set_uniform(std::string_view name, const Matrix4& mat) {
    const GLint loc = uniform_location(name);
    glUniformMatrix4fv(loc, 1, GL_TRUE, mat.data());
}

void Shader::set_uniform(std::string_view name, const Vector3& vec) {
    impl->set_uniform(name, vec);
}
void Shader::Impl::set_uniform(std::string_view name, const Vector3& vec) {
    const GLint loc = uniform_location(name);
    glUniform3fv(loc, 1, vec.data());
}

GLint Shader::Impl::uniform_location(std::string_view name) {
    const auto it = uniform_locations.find(name.data());
    if (it == uniform_locations.end()) {
        // Cache miss, get and insert
        const GLint loc = glGetUniformLocation(program, static_cast<const GLchar*>(name.data()));
        if (loc == -1) {
            throw std::runtime_error(std::string("Uniform with name ") + name.data() +
                                     " not found in shader program\n");
        }
        uniform_locations.insert({name.data(), loc});
        return loc;
    } else {
        // Cache hit, return cached value
        return it->second;
    }
}
