#include "viewer.hpp"

struct Viewer::Impl {
    Vector3 eye, center, up;
    Impl(Vector3 eye, Vector3 center, Vector3 up) : eye(eye), center(center), up(up) {}
    Matrix4 matrix() const;
};

Matrix4 Viewer::Impl::matrix() const {
    // new z axis, outwards from screen
    const Vector3 z_axis = (center - eye).normalize();
    const Vector3 x_axis = z_axis.cross(up).normalize();
    const Vector3 y_axis = x_axis.cross(z_axis).normalize();

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

Viewer::Viewer(Vector3 pos, Vector3 center, Vector3 up)
    : impl(std::make_unique<Impl>(pos, center, up)) {}
Viewer::~Viewer() = default;
Matrix4 Viewer::matrix() const { return impl->matrix(); }

void Viewer::change_eyepos(Vector3 delta) { impl->eye += delta; }
void Viewer::change_up(Vector3 delta) { impl->up += delta; }
void Viewer::change_center(Vector3 delta) { impl->center += delta; }
