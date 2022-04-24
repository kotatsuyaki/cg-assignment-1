#ifndef MVP_HPP_
#define MVP_HPP_

#include <memory>

#include "transform.hpp"

// Wrapper for MVP transformation.
class Mvp : public Transform {
  public:
    explicit Mvp(int initial_width, int initial_height);
    ~Mvp();
    virtual Matrix4 matrix() const override;

    Mvp(const Mvp&) = delete;
    Mvp& operator=(const Mvp&) = delete;
    Mvp(Mvp&&) = default;
    Mvp& operator=(Mvp&&) = default;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
