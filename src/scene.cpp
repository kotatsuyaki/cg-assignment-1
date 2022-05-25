#include "scene.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>

#include <glad/glad.h>

#include "drawable.hpp"
#include "matrix.hpp"
#include "shader.hpp"
#include "transform/transform.hpp"

// The blue-greenish plane to be rendered below the model.
class Quad : public Drawable {
  public:
    Quad();
    ~Quad();
    virtual void draw(Shader& shader) const override;

    static const GLsizei VERTEX_COUNT;

  private:
    GLuint vao;
    GLuint vertices;
    GLuint colors;
};

class Scene::Impl {
  public:
    Impl(Shader shader, std::unique_ptr<Drawable> drawable)
        : shader(std::move(shader)), drawable(std::move(drawable)), mode(RenderMode::Solid),
          quad() {}

    class RenderMode {
      public:
        enum Value { Solid, Wireframe };
        RenderMode(Value value) : value(value) {}
        GLenum mode() const {
            constexpr std::array<GLenum, 2> data{GL_FILL, GL_LINE};
            return data[value];
        }
        constexpr operator Value() const { return value; }
        explicit operator bool() = delete;

      private:
        Value value;
    };

    void render(const Window& window, StagedTransform& transform);
    void switch_render_mode();

  private:
    Shader shader;
    RenderMode mode;
    std::unique_ptr<Drawable> drawable;
    Quad quad;

    void draw_model(const StagedTransform& transform);
};

Scene::Scene(Shader shader, std::unique_ptr<Drawable> drawable)
    : impl(std::make_unique<Impl>(std::move(shader), std::move(drawable))) {}
Scene::~Scene() = default;

void Scene::render(const Window& window, StagedTransform& transform) {
    impl->render(window, transform);
}
void Scene::Impl::render(const Window& window, StagedTransform& transform) {
    window.make_current();

    // Draw model with user-specified mode
    glPolygonMode(GL_FRONT_AND_BACK, mode.mode());
    draw_model(transform);
}

void Scene::Impl::draw_model(const StagedTransform& transform) {
    const Matrix4 m = transform.model_matrix();
    const Matrix4 vp = transform.view_project_matrix();

    shader.set_uniform("m", m);
    shader.set_uniform("vp", vp);
    drawable->draw(shader);
}

void Scene::switch_render_mode() { impl->switch_render_mode(); }
void Scene::Impl::switch_render_mode() {
    switch (mode) {
    case RenderMode::Solid:
        mode = RenderMode::Wireframe;
        break;
    case RenderMode::Wireframe:
        mode = RenderMode::Solid;
        break;
    }
}

Quad::Quad() {
    const std::array<float, 18> vertices{1.0f,  -0.9f, -1.0f, //
                                         1.0f,  -0.9f, 1.0f,  //
                                         -1.0f, -0.9f, -1.0f, //
                                         1.0f,  -0.9f, 1.0f,  //
                                         -1.0f, -0.9f, 1.0f,  //
                                         -1.0f, -0.9f, -1.0f};
    const std::array<float, 18> colors{0.0f, 1.0f, 0.0f, //
                                       0.0f, 0.5f, 0.8f, //
                                       0.0f, 1.0f, 0.0f, //
                                       0.0f, 0.5f, 0.8f, //
                                       0.0f, 0.5f, 0.8f, //
                                       0.0f, 1.0f, 0.0f};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &this->vertices);
    glBindBuffer(GL_ARRAY_BUFFER, (this->vertices));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.at(0),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &this->colors);
    glBindBuffer(GL_ARRAY_BUFFER, this->colors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors.at(0), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

Quad::~Quad() {
    glDeleteBuffers(1, &vertices);
    glDeleteBuffers(1, &colors);
    glDeleteVertexArrays(1, &vao);
}

void Quad::draw(Shader& shader) const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, Quad::VERTEX_COUNT);
}

const GLsizei Quad::VERTEX_COUNT = 6;
