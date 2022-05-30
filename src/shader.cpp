#include "shader.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include "window.hpp"

namespace {
enum class MagFilterMode {
    Nearest,
    Linear,
};

enum class MinFilterMode {
    NearestMipmapLinear,
    LinearMipmapLinear,
};

} // namespace

class Shader::Impl {
  public:
    Impl(GLuint program) : program(program), uniform_locations() {}
    ~Impl() { glDeleteProgram(program); }

    void set_uniform(std::string_view name, const Matrix4& mat);
    void set_uniform(std::string_view name, const Vector3& vec);
    void set_uniform(std::string_view name, GLint value);
    void set_uniform(std::string_view name, GLfloat value);

    GLint uniform_location(std::string_view name);

    MagFilterMode mag_filter_mode = MagFilterMode::Nearest;
    MinFilterMode min_filter_mode = MinFilterMode::NearestMipmapLinear;

    void toggle_mag_filter();
    void toggle_min_filter();

    int eye_offset_index = 0;

    void next_eye_offset();
    void prev_eye_offset();

  private:
    GLuint program;

    // Internal cache for uniform locations
    std::unordered_map<std::string, GLint> uniform_locations;
};

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

    impl = std::make_shared<Impl>(p);
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

void Shader::set_uniform(std::string_view name, GLint value) { impl->set_uniform(name, value); }
void Shader::Impl::set_uniform(std::string_view name, GLint value) {
    const GLint loc = uniform_location(name);
    glUniform1i(loc, value);
}

void Shader::set_uniform(std::string_view name, GLfloat value) { impl->set_uniform(name, value); }
void Shader::Impl::set_uniform(std::string_view name, GLfloat value) {
    const GLint loc = uniform_location(name);
    glUniform1f(loc, value);
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

void Shader::toggle_mag_filter() { impl->toggle_mag_filter(); }
void Shader::Impl::toggle_mag_filter() {
    if (mag_filter_mode == MagFilterMode::Nearest) {
        mag_filter_mode = MagFilterMode::Linear;
    } else {
        mag_filter_mode = MagFilterMode::Nearest;
    }
}

void Shader::toggle_min_filter() { impl->toggle_min_filter(); }
void Shader::Impl::toggle_min_filter() {
    if (min_filter_mode == MinFilterMode::NearestMipmapLinear) {
        min_filter_mode = MinFilterMode::LinearMipmapLinear;
    } else {
        min_filter_mode = MinFilterMode::NearestMipmapLinear;
    }
}

GLint Shader::mag_filter() const {
    if (impl->mag_filter_mode == MagFilterMode::Nearest) {
        return GL_NEAREST;
    } else {
        return GL_LINEAR;
    }
}
GLint Shader::min_filter() const {
    if (impl->min_filter_mode == MinFilterMode::NearestMipmapLinear) {
        return GL_NEAREST_MIPMAP_LINEAR;
    } else {
        return GL_LINEAR_MIPMAP_NEAREST;
    }
}

void Shader::Impl::next_eye_offset() { eye_offset_index = (eye_offset_index + 1) % 7; }
void Shader::next_eye_offset() { impl->next_eye_offset(); }
void Shader::Impl::prev_eye_offset() {
    eye_offset_index = (eye_offset_index == 0) ? (7 - 1) : (eye_offset_index - 1);
}
void Shader::prev_eye_offset() { impl->prev_eye_offset(); }
GLint Shader::eye_offset_index() const { return impl->eye_offset_index; }
