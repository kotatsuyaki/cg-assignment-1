#ifndef MVP_HPP_
#define MVP_HPP_

#include <memory>

#include "transform.hpp"

class Mvp : public Transform {
  public:
    Mvp(int initial_width, int initial_height);
    ~Mvp();
    virtual Matrix4 matrix() const override;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
