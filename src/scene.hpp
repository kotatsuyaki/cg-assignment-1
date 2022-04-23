#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>

#include "drawable.hpp"
#include "matrix.hpp"
#include "shader.hpp"
#include "vector.hpp"
#include "window.hpp"

#include "glad/glad.h"

class Projection final {
  public:
    ~Projection();
    Matrix4 matrix() const;

  private:
    struct ProjectionImpl;
    std::unique_ptr<ProjectionImpl> impl;

    Projection(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect, GLfloat left,
               GLfloat right, GLfloat top, GLfloat bottom);
    friend class ProjectionBuilder;
};

class ProjectionBuilder final {
  public:
    ProjectionBuilder();
    ~ProjectionBuilder();

    ProjectionBuilder& with_near_clip(GLfloat value);
    ProjectionBuilder& with_far_clip(GLfloat value);
    ProjectionBuilder& with_fovy(GLfloat value);
    ProjectionBuilder& with_aspect(GLfloat value);
    ProjectionBuilder& with_left(GLfloat value);
    ProjectionBuilder& with_right(GLfloat value);
    ProjectionBuilder& with_top(GLfloat value);
    ProjectionBuilder& with_bottom(GLfloat value);

    // Builds an instance of Projection.
    // Throws if there are values not assigned yet.
    Projection build() const;

  private:
    struct ProjectionBuilderImpl;
    std::unique_ptr<ProjectionBuilderImpl> impl;
};

class Camera final {
  public:
    Camera(Vector3 pos, Vector3 center, Vector3 up);
    ~Camera();
    Matrix4 matrix() const;

    // Prevent copy, allow move
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = default;
    Camera& operator=(Camera&&) = default;

  private:
    struct CameraImpl;
    std::unique_ptr<CameraImpl> impl;
};

// TODO: Break scene into layers
class Scene final {
  public:
    Scene(Shader shader, Vector3 clear_color, std::unique_ptr<Drawable> drawable);
    ~Scene();

    // Prevent copy, allow move
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    void render(const Window& window, const Projection& proj, const Camera& cam);

  private:
    struct SceneImpl;
    std::unique_ptr<SceneImpl> impl;
};

#endif
