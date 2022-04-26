#include "scene.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>

#include <glad/glad.h>

#include "matrix.hpp"
#include "shader.hpp"
#include "transform/transform.hpp"

struct Scene::Impl {
    Impl(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
        : shader(std::move(shader)), clear_color(clear_color), drawable(std::move(drawable)),
          mode(RenderMode::Solid) {}

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

    void render(const Window& window, const Transform& transform);
    void switch_render_mode();

    Shader shader;
    Vector3 clear_color;
    RenderMode mode;
    std::unique_ptr<Drawable> drawable;
};

Scene::Scene(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
    : impl(std::make_unique<Impl>(std::move(shader), clear_color, std::move(drawable))) {}
Scene::~Scene() = default;

void Scene::render(const Window& window, const Transform& transform) {
    impl->render(window, transform);
}
void Scene::Impl::render(const Window& window, const Transform& transform) {
    window.make_current();

    // clear canvas
    glEnable(GL_DEPTH_TEST);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, mode.mode());

    Matrix4 t, r, s;
    // [TODO] update translation, rotation and scaling

    Matrix4 mvp = transform.matrix();

    // Update mvp and draw the model
    shader.set_uniform("mvp", mvp);
    drawable->draw();

    // TODO: Implement this
    // draw_plane();
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
