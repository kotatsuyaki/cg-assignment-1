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
    virtual void draw() const override;

    static const GLsizei VERTEX_COUNT;

  private:
    GLuint vao;
    GLuint vertices;
    GLuint colors;
};

class Scene::Impl {
  public:
    Impl(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
        : shader(std::move(shader)), clear_color(clear_color), drawable(std::move(drawable)),
          mode(RenderMode::Solid), quad() {}

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
    Vector3 clear_color;
    RenderMode mode;
    std::unique_ptr<Drawable> drawable;
    Quad quad;

    void draw_model(const StagedTransform& transform);
    void draw_floor(const StagedTransform& transform);
};

Scene::Scene(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
    : impl(std::make_unique<Impl>(std::move(shader), clear_color, std::move(drawable))) {}
Scene::~Scene() = default;

void Scene::render(const Window& window, StagedTransform& transform) {
    impl->render(window, transform);
}
void Scene::Impl::render(const Window& window, StagedTransform& transform) {
    window.make_current();

    // clear canvas
    glEnable(GL_DEPTH_TEST);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, mode.mode());

    // Draw model and floor
    draw_model(transform);
    draw_floor(transform);
}

void Scene::Impl::draw_model(const StagedTransform& transform) {
    const Matrix4 mvp = transform.matrix();

    shader.set_uniform("mvp", mvp);
    drawable->draw();
}

void Scene::Impl::draw_floor(const StagedTransform& transform) {
    const Matrix4 vp = transform.view_project_matrix();
    shader.set_uniform("mvp", vp);
    quad.draw();
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

void Quad::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, Quad::VERTEX_COUNT);
}

const GLsizei Quad::VERTEX_COUNT = 6;
