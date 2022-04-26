#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>

#include "drawable.hpp"
#include "shader.hpp"
#include "transform/transform.hpp"
#include "window.hpp"

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

    void render(const Window& window, const Transform& transform);

    void switch_render_mode();

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
