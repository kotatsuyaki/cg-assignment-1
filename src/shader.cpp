#include "shader.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include "window.hpp"

struct Shader::ShaderImpl {
    GLuint program;

    // Internal cache for uniform locations
    std::unordered_map<std::string, GLint> uniform_locations;

    ShaderImpl(GLuint program) : program(program), uniform_locations() {}
    ~ShaderImpl() { glDeleteProgram(program); }

    ShaderImpl(const ShaderImpl&) = delete;
    ShaderImpl& operator=(const ShaderImpl&) = delete;
};
void Shader::ShaderImplDeleter::operator()(ShaderImpl* ptr) const { delete ptr; }

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

    impl = std::unique_ptr<ShaderImpl, ShaderImplDeleter>(new ShaderImpl(p));
}

Shader::~Shader() {}

GLint Shader::uniform_location(std::string_view name) {
    const auto it = impl->uniform_locations.find(name.data());
    if (it == impl->uniform_locations.end()) {
        // Cache miss, get and insert
        GLint loc = glGetUniformLocation(impl->program, static_cast<const GLchar*>(name.data()));
        impl->uniform_locations.insert({name.data(), loc});
        return loc;
    } else {
        // Cache hit, return cached value
        return it->second;
    }
}
