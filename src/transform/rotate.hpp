#ifndef ROTATE_HPP_
#define ROTATE_HPP_

#include <memory>

#include "../matrix.hpp"
#include "../vector.hpp"
#include "transform.hpp"

// Rotation transform
class Rotate final : public Transform {
  public:
    explicit Rotate(Vector3 offset);
    ~Rotate();
    virtual Matrix4 matrix() const override;

    /// Changes the scale by amount `delta`
    void change(const Vector3& delta);

    // Prevent copy, allow move
    Rotate(const Rotate&) = delete;
    Rotate& operator=(const Rotate&) = delete;
    Rotate(Rotate&&);
    Rotate& operator=(Rotate&&);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif
