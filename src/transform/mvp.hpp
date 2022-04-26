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

    void set_viewport_size(int width, int height);

    enum class ProjectMode { Perspective, Orthogonal };
    void set_project_mode(ProjectMode mode);

    // Prints the underlying matrices
    void debug_print() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
