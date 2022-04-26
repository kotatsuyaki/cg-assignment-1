#ifndef SCALE_HPP_
#define SCALE_HPP_

#include <memory>

#include "../matrix.hpp"
#include "../vector.hpp"
#include "transform.hpp"

// Scaling transform
class Scale final : public Transform {
  public:
    explicit Scale(Vector3 offset);
    ~Scale();
    virtual Matrix4 matrix() const override;

    /// Changes the scale by amount `delta`
    void change(const Vector3& delta);

    // Prevent copy, allow move
    Scale(const Scale&) = delete;
    Scale& operator=(const Scale&) = delete;
    Scale(Scale&&);
    Scale& operator=(Scale&&);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif
