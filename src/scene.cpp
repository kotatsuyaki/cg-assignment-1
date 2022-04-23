#include "scene.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <stdexcept>

#include "matrix.hpp"
#include "shader.hpp"

struct ProjConfig::ProjConfigImpl {
    GLfloat near_clip;
    GLfloat far_clip;
    GLfloat fovy;
    GLfloat aspect;
    GLfloat left;
    GLfloat right;
    GLfloat top;
    GLfloat bottom;

    ProjConfigImpl(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect, GLfloat left,
                   GLfloat right, GLfloat top, GLfloat bottom)
        : near_clip(near_clip), far_clip(far_clip), fovy(fovy), aspect(aspect), left(left),
          right(right), top(top), bottom(bottom) {}
};

ProjConfig::ProjConfig(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect,
                       GLfloat left, GLfloat right, GLfloat top, GLfloat bottom)
    : impl(std::make_unique<ProjConfigImpl>(near_clip, far_clip, fovy, aspect, left, right, top,
                                            bottom)) {}
ProjConfig::~ProjConfig() = default;

struct ProjConfigBuilder::ProjConfigBuilderImpl {
    std::optional<GLfloat> near_clip;
    std::optional<GLfloat> far_clip;
    std::optional<GLfloat> fovy;
    std::optional<GLfloat> aspect;
    std::optional<GLfloat> left;
    std::optional<GLfloat> right;
    std::optional<GLfloat> top;
    std::optional<GLfloat> bottom;
};

ProjConfigBuilder::ProjConfigBuilder() { impl = std::make_unique<ProjConfigBuilderImpl>(); }
ProjConfigBuilder::~ProjConfigBuilder() = default;

void ProjConfigBuilder::with_near_clip(GLfloat value) { impl->near_clip = value; }
void ProjConfigBuilder::with_far_clip(GLfloat value) { impl->far_clip = value; }
void ProjConfigBuilder::with_fovy(GLfloat value) { impl->fovy = value; }
void ProjConfigBuilder::with_aspect(GLfloat value) { impl->aspect = value; }
void ProjConfigBuilder::with_left(GLfloat value) { impl->left = value; }
void ProjConfigBuilder::with_right(GLfloat value) { impl->right = value; }
void ProjConfigBuilder::with_top(GLfloat value) { impl->top = value; }
void ProjConfigBuilder::with_bottom(GLfloat value) { impl->bottom = value; }

ProjConfig ProjConfigBuilder::build() const {
    std::array<std::optional<GLfloat>, 8> opts = {impl->near_clip, impl->far_clip, impl->fovy,
                                                  impl->aspect,    impl->left,     impl->right,
                                                  impl->top,       impl->bottom};
    const bool has_unset = std::any_of(opts.begin(), opts.end(), [](std::optional<GLfloat> opt) {
        return opt.has_value() == false;
    });
    if (has_unset) {
        throw std::runtime_error("Building ProjConfig failed due to unset field(s)");
    }

    return ProjConfig{*impl->near_clip, *impl->far_clip, *impl->fovy, *impl->aspect,
                      *impl->left,      *impl->right,    *impl->top,  *impl->bottom};
}

struct Scene::SceneImpl {
    SceneImpl(Shader shader, Vector3 clear_color)
        : shader(std::move(shader)), clear_color(clear_color) {}
    Shader shader;
    Vector3 clear_color;
};

Scene::Scene(Shader shader, Vector3 clear_color)
    : impl(std::make_unique<SceneImpl>(std::move(shader), clear_color)) {}
Scene::~Scene() = default;

void Scene::render(const Window& window, const Model& model) {
    window.make_current();

    // clear canvas
    glEnable(GL_DEPTH_TEST);
    glClearColor(impl->clear_color.x, impl->clear_color.y, impl->clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix4 t, r, s;
    // [TODO] update translation, rotation and scaling

    Matrix4 mvp;
    GLfloat mvp_arr[16];

    // [TODO] multiply all the matrix
    // [TODO] row-major ---> column-major

    mvp_arr[0] = 1;
    mvp_arr[4] = 0;
    mvp_arr[8] = 0;
    mvp_arr[12] = 0;
    mvp_arr[1] = 0;
    mvp_arr[5] = 1;
    mvp_arr[9] = 0;
    mvp_arr[13] = 0;
    mvp_arr[2] = 0;
    mvp_arr[6] = 0;
    mvp_arr[10] = 1;
    mvp_arr[14] = 0;
    mvp_arr[3] = 0;
    mvp_arr[7] = 0;
    mvp_arr[11] = 0;
    mvp_arr[15] = 1;

    // use uniform to send mvp to vertex shader
    auto mvp_loc = impl->shader.uniform_location("mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_arr);
    model.draw();

    // TODO: Implement this
    // draw_plane();
}
