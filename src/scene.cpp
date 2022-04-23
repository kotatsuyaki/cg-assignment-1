#include "scene.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>

#include "matrix.hpp"
#include "shader.hpp"

constexpr float PI = 3.14159265358979323846;

constexpr float deg2rad(float deg) { return deg * PI / 180.0f; }

struct Projection::ProjectionImpl {
    GLfloat near_clip;
    GLfloat far_clip;
    GLfloat fovy;
    GLfloat aspect;
    GLfloat left;
    GLfloat right;
    GLfloat top;
    GLfloat bottom;

    ProjectionImpl(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect, GLfloat left,
                   GLfloat right, GLfloat top, GLfloat bottom)
        : near_clip(near_clip), far_clip(far_clip), fovy(fovy), aspect(aspect), left(left),
          right(right), top(top), bottom(bottom) {}
    Matrix4 matrix() const;
};

Projection::Projection(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect,
                       GLfloat left, GLfloat right, GLfloat top, GLfloat bottom)
    : impl(std::make_unique<ProjectionImpl>(near_clip, far_clip, fovy, aspect, left, right, top,
                                            bottom)) {}
Projection::~Projection() = default;
Matrix4 Projection::matrix() const { return impl->matrix(); }

Matrix4 Projection::ProjectionImpl::matrix() const {
    const float tan_term = std::tan(deg2rad(fovy) / 2.0f);

    const float x_scale = 1.0f / (tan_term * aspect);
    const float y_scale = 1.0f / tan_term;
    const float z_scale_a = (near_clip + far_clip) / (near_clip - far_clip);
    const float z_scale_b = 2 * near_clip * far_clip / (near_clip - far_clip);

    return Matrix4{x_scale, 0,       0,         0,         //
                   0,       y_scale, 0,         0,         //
                   0,       0,       z_scale_a, z_scale_b, //
                   0,       0,       -1,        0};
}

struct ProjectionBuilder::ProjectionBuilderImpl {
    std::optional<GLfloat> near_clip;
    std::optional<GLfloat> far_clip;
    std::optional<GLfloat> fovy;
    std::optional<GLfloat> aspect;
    std::optional<GLfloat> left;
    std::optional<GLfloat> right;
    std::optional<GLfloat> top;
    std::optional<GLfloat> bottom;
};

ProjectionBuilder::ProjectionBuilder() { impl = std::make_unique<ProjectionBuilderImpl>(); }
ProjectionBuilder::~ProjectionBuilder() = default;

ProjectionBuilder& ProjectionBuilder::with_near_clip(GLfloat value) {
    impl->near_clip = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_far_clip(GLfloat value) {
    impl->far_clip = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_fovy(GLfloat value) {
    impl->fovy = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_aspect(GLfloat value) {
    impl->aspect = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_left(GLfloat value) {
    impl->left = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_right(GLfloat value) {
    impl->right = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_top(GLfloat value) {
    impl->top = value;
    return *this;
}
ProjectionBuilder& ProjectionBuilder::with_bottom(GLfloat value) {
    impl->bottom = value;
    return *this;
}

Projection ProjectionBuilder::build() const {
    std::array<std::optional<GLfloat>, 8> opts = {impl->near_clip, impl->far_clip, impl->fovy,
                                                  impl->aspect,    impl->left,     impl->right,
                                                  impl->top,       impl->bottom};
    const bool has_unset = std::any_of(opts.begin(), opts.end(), [](std::optional<GLfloat> opt) {
        return opt.has_value() == false;
    });
    if (has_unset) {
        throw std::runtime_error("Building Projection failed due to unset field(s)");
    }

    return Projection{*impl->near_clip, *impl->far_clip, *impl->fovy, *impl->aspect,
                      *impl->left,      *impl->right,    *impl->top,  *impl->bottom};
}

struct Camera::CameraImpl {
    Vector3 eye, center, up;
    CameraImpl(Vector3 eye, Vector3 center, Vector3 up) : eye(eye), center(center), up(up) {}
    Matrix4 matrix() const;
};

Matrix4 Camera::CameraImpl::matrix() const {
    // up is assumed to be a unit vector

    // new z axis, outwards from screen
    const Vector3 z_axis = (center - eye).normalize();
    const Vector3 x_axis = z_axis.cross(up);
    const Vector3 y_axis = up;

    const Matrix4 rot{x_axis.x,  x_axis.y,  x_axis.z,  0, //
                      y_axis.x,  y_axis.y,  y_axis.z,  0, //
                      -z_axis.x, -z_axis.y, -z_axis.z, 0, //
                      0,         0,         0,         1};
    const Matrix4 trans{1, 0, 0, -eye.x, //
                        0, 1, 0, -eye.y, //
                        0, 0, 1, -eye.z, //
                        0, 0, 0, 1};

    return rot * trans;
}

Camera::Camera(Vector3 pos, Vector3 center, Vector3 up)
    : impl(std::make_unique<CameraImpl>(pos, center, up)) {}
Camera::~Camera() = default;
Matrix4 Camera::matrix() const { return impl->matrix(); }

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

void Scene::render(const Window& window, const Projection& proj, const Camera& cam) {
    window.make_current();

    // clear canvas
    glEnable(GL_DEPTH_TEST);
    glClearColor(impl->clear_color.x, impl->clear_color.y, impl->clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix4 t, r, s;
    // [TODO] update translation, rotation and scaling

    Matrix4 mvp_colmaj = (proj.matrix() * cam.matrix()).transpose();

    // Update mvp to shader
    auto mvp_loc = impl->shader.uniform_location("mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp_colmaj.data());

    impl->drawable->draw();

    // TODO: Implement this
    // draw_plane();
}
