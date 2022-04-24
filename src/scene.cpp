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

struct Scene::SceneImpl {
    SceneImpl(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
        : shader(std::move(shader)), clear_color(clear_color), drawable(std::move(drawable)) {}
    Shader shader;
    Vector3 clear_color;
    std::unique_ptr<Drawable> drawable;
};

Scene::Scene(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable)
    : impl(std::make_unique<SceneImpl>(std::move(shader), clear_color, std::move(drawable))) {}
Scene::~Scene() = default;

void Scene::render(const Window& window, const Transform& transform) {
    window.make_current();

    // clear canvas
    glEnable(GL_DEPTH_TEST);
    glClearColor(impl->clear_color.x, impl->clear_color.y, impl->clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix4 t, r, s;
    // [TODO] update translation, rotation and scaling

    Matrix4 mvp = transform.matrix();

    // Update mvp and draw the model
    impl->shader.set_uniform("mvp", mvp);
    impl->drawable->draw();

    // TODO: Implement this
    // draw_plane();
}
