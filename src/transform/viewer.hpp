#ifndef VIEWER_HPP_
#define VIEWER_HPP_

#include <memory>

#include "../matrix.hpp"
#include "../vector.hpp"
#include "transform.hpp"

// Viewing transform (i.e. camera).
class Viewer final : public Transform {
  public:
    explicit Viewer(Vector3 pos, Vector3 center, Vector3 up);
    ~Viewer();
    virtual Matrix4 matrix() const override;

    // Explicit copy, allow move
    Viewer(const Viewer&) = delete;
    Viewer& operator=(const Viewer&) = delete;
    Viewer(Viewer&&) = default;
    Viewer& operator=(Viewer&&) = default;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
