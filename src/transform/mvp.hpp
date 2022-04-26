#ifndef MVP_HPP_
#define MVP_HPP_

#include <memory>

#include "transform.hpp"

// Wrapper for MVP transformation.
class Mvp : public Transform {
  public:
    explicit Mvp(int width, int height);
    ~Mvp();
    virtual Matrix4 matrix() const override;

    Mvp(const Mvp&) = delete;
    Mvp& operator=(const Mvp&) = delete;
    Mvp(Mvp&&) = default;
    Mvp& operator=(Mvp&&) = default;

    // Updates the viewport size.
    void set_viewport_size(int width, int height);

    enum class ProjectMode { Perspective, Orthogonal };

    // Updates the projection mode.
    void set_project_mode(ProjectMode mode);

    // Prints the underlying matrices.
    void debug_print() const;

    // Updates the model transformation parameters.
    void update_translation(Vector3 delta);
    void update_rotation(Vector3 delta);
    void update_scaling(Vector3 delta);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif
