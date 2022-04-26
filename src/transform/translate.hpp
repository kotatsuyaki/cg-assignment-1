#ifndef TRANSLATE_HPP_
#define TRANSLATE_HPP_

#include <memory>

#include "../matrix.hpp"
#include "../vector.hpp"
#include "transform.hpp"

// Translating transform
class Translate final : public Transform {
  public:
    explicit Translate(Vector3 offset);
    ~Translate();
    virtual Matrix4 matrix() const override;

    /// Changes the position by amount `delta`
    void change(const Vector3& delta);

    // Prevent copy, allow move
    Translate(const Translate&) = delete;
    Translate& operator=(const Translate&) = delete;
    Translate(Translate&&);
    Translate& operator=(Translate&&);

  private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

#endif
