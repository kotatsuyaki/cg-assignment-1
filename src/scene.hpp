#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>

#include "drawable.hpp"
#include "shader.hpp"
#include "vector.hpp"
#include "window.hpp"

#include "glad/glad.h"

class ProjConfig final {
  public:
    ProjConfig(GLfloat near_clip, GLfloat far_clip, GLfloat fovy, GLfloat aspect, GLfloat left,
               GLfloat right, GLfloat top, GLfloat bottom);
    ~ProjConfig();

    // Prevent copy, allow move
    ProjConfig(const ProjConfig&) = delete;
    ProjConfig& operator=(const ProjConfig&) = delete;
    ProjConfig(ProjConfig&&) = default;
    ProjConfig& operator=(ProjConfig&&) = default;

  private:
    struct ProjConfigImpl;
    std::unique_ptr<ProjConfigImpl> impl;
};

class ProjConfigBuilder final {
  public:
    ProjConfigBuilder();
    ~ProjConfigBuilder();

    // Prevent copy, allow move
    ProjConfigBuilder(const ProjConfigBuilder&) = delete;
    ProjConfigBuilder& operator=(const ProjConfigBuilder&) = delete;
    ProjConfigBuilder(ProjConfigBuilder&&) = default;
    ProjConfigBuilder& operator=(ProjConfigBuilder&&) = default;

    void with_near_clip(GLfloat value);
    void with_far_clip(GLfloat value);
    void with_fovy(GLfloat value);
    void with_aspect(GLfloat value);
    void with_left(GLfloat value);
    void with_right(GLfloat value);
    void with_top(GLfloat value);
    void with_bottom(GLfloat value);

    // Builds an instance of ProjConfig.
    // Throws if there are values not assigned yet.
    ProjConfig build() const;

  private:
    struct ProjConfigBuilderImpl;
    std::unique_ptr<ProjConfigBuilderImpl> impl;
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

    void render(const Window& window);

  private:
    struct SceneImpl;
    std::unique_ptr<SceneImpl> impl;
};

#endif
