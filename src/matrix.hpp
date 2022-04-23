///////////////////////////////////////////////////////////////////////////////
// Matrice.h
// =========
// NxN Matrix Math classes
//
// All matrices are row major. (OpenGL uses column-major matrix)
// | 0 1 |    | 0 1 2 |    |  0  1  2  3 |
// | 2 3 |    | 3 4 5 |    |  4  5  6  7 |
//            | 6 7 8 |    |  8  9 10 11 |
//                         | 12 13 14 15 |
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-06-24
// UPDATED: 2012-05-29
//
// Copyright (C) 2005 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////

#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include "vector.hpp"

#include <ostream>

///////////////////////////////////////////////////////////////////////////
// 2x2 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix2 {
  public:
    // constructors
    Matrix2(); // init with identity
    Matrix2(const float src[4]);
    Matrix2(float xx, float xy, float yx, float yy);

    void set(const float src[4]);
    void set(float xx, float xy, float yx, float yy);
    void set_row(int index, const float row[2]);
    void set_row(int index, const Vector2& v);
    void set_column(int index, const float col[2]);
    void set_column(int index, const Vector2& v);

    const float* data() const;
    float get_determinant();

    Matrix2& identity();
    Matrix2& transpose(); // transpose itself and return reference
    Matrix2& invert();

    // operators
    Matrix2 operator+(const Matrix2& rhs) const; // add rhs
    Matrix2 operator-(const Matrix2& rhs) const; // subtract rhs
    Matrix2& operator+=(const Matrix2& rhs);     // add rhs and update this object
    Matrix2& operator-=(const Matrix2& rhs);     // subtract rhs and update this object
    Vector2 operator*(const Vector2& rhs) const; // multiplication: v' = M * v
    Matrix2 operator*(const Matrix2& rhs) const; // multiplication: M3 = M1 * M2
    Matrix2& operator*=(const Matrix2& rhs);     // multiplication: M1' = M1 * M2
    bool operator==(const Matrix2& rhs) const;   // exact compare, no epsilon
    bool operator!=(const Matrix2& rhs) const;   // exact compare, no epsilon
    float operator[](int index) const;           // subscript operator v[0], v[1]
    float& operator[](int index);                // subscript operator v[0], v[1]

    friend Matrix2 operator-(const Matrix2& m); // unary operator (-)
    friend Matrix2 operator*(float scalar,
                             const Matrix2& m); // pre-multiplication
    friend Vector2 operator*(const Vector2& vec,
                             const Matrix2& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix2& m);

  protected:
  private:
    float m[4];
};

///////////////////////////////////////////////////////////////////////////
// 3x3 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix3 {
  public:
    // constructors
    Matrix3(); // init with identity
    Matrix3(const float src[9]);
    Matrix3(float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy,
            float zz);

    void set(const float src[9]);
    void set(float xx, float xy, float xz, float yx, float yy, float yz, float zx, float zy,
             float zz);
    void set_row(int index, const float row[3]);
    void set_row(int index, const Vector3& v);
    void set_column(int index, const float col[3]);
    void set_column(int index, const Vector3& v);

    const float* data() const;
    float get_determinant();

    Matrix3& identity();
    Matrix3& transpose(); // transpose itself and return reference
    Matrix3& invert();

    // operators
    Matrix3 operator+(const Matrix3& rhs) const; // add rhs
    Matrix3 operator-(const Matrix3& rhs) const; // subtract rhs
    Matrix3& operator+=(const Matrix3& rhs);     // add rhs and update this object
    Matrix3& operator-=(const Matrix3& rhs);     // subtract rhs and update this object
    Vector3 operator*(const Vector3& rhs) const; // multiplication: v' = M * v
    Matrix3 operator*(const Matrix3& rhs) const; // multiplication: M3 = M1 * M2
    Matrix3& operator*=(const Matrix3& rhs);     // multiplication: M1' = M1 * M2
    bool operator==(const Matrix3& rhs) const;   // exact compare, no epsilon
    bool operator!=(const Matrix3& rhs) const;   // exact compare, no epsilon
    float operator[](int index) const;           // subscript operator v[0], v[1]
    float& operator[](int index);                // subscript operator v[0], v[1]

    friend Matrix3 operator-(const Matrix3& m); // unary operator (-)
    friend Matrix3 operator*(float scalar,
                             const Matrix3& m); // pre-multiplication
    friend Vector3 operator*(const Vector3& vec,
                             const Matrix3& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix3& m);

  protected:
  private:
    float m[9];
};

///////////////////////////////////////////////////////////////////////////
// 4x4 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix4 {
  public:
    // constructors
    Matrix4(); // init with identity
    Matrix4(const float src[16]);
    Matrix4(float xx, float xy, float xz, float xw, float yx, float yy, float yz, float yw,
            float zx, float zy, float zz, float zw, float wx, float wy, float wz, float ww);

    void set(const float src[16]);
    void set(float xx, float xy, float xz, float xw, float yx, float yy, float yz, float yw,
             float zx, float zy, float zz, float zw, float wx, float wy, float wz, float ww);
    void set_row(int index, const float row[4]);
    void set_row(int index, const Vector4& v);
    void set_row(int index, const Vector3& v);
    void set_column(int index, const float col[4]);
    void set_column(int index, const Vector4& v);
    void set_column(int index, const Vector3& v);

    const float* data() const;
    float get_determinant();

    Matrix4& identity();
    Matrix4& transpose();         // transpose itself and return reference
    Matrix4& invert();            // check best inverse method before inverse
    Matrix4& invert_euclidean();  // inverse of Euclidean transform matrix
    Matrix4& invert_affine();     // inverse of affine transform matrix
    Matrix4& invert_projective(); // inverse of projective matrix using partitioning
    Matrix4& invert_general();    // inverse of generic matrix

    // transform matrix
    Matrix4& translate(float x, float y, float z); // translation by (x,y,z)
    Matrix4& translate(const Vector3& v);          //
    Matrix4& rotate(float angle,
                    const Vector3& axis); // rotate angle(degree) along the given axix
    Matrix4& rotate(float angle, float x, float y, float z);
    Matrix4& rotate_x(float angle); // rotate on X-axis with degree
    Matrix4& rotate_y(float angle); // rotate on Y-axis with degree
    Matrix4& rotate_z(float angle); // rotate on Z-axis with degree
    Matrix4& scale(float scale);    // uniform scale
    Matrix4& scale(float sx, float sy,
                   float sz); // scale by (sx, sy, sz) on each axis

    // operators
    Matrix4 operator+(const Matrix4& rhs) const; // add rhs
    Matrix4 operator-(const Matrix4& rhs) const; // subtract rhs
    Matrix4& operator+=(const Matrix4& rhs);     // add rhs and update this object
    Matrix4& operator-=(const Matrix4& rhs);     // subtract rhs and update this object
    Vector4 operator*(const Vector4& rhs) const; // multiplication: v' = M * v
    Vector3 operator*(const Vector3& rhs) const; // multiplication: v' = M * v
    Matrix4 operator*(const Matrix4& rhs) const; // multiplication: M3 = M1 * M2
    Matrix4& operator*=(const Matrix4& rhs);     // multiplication: M1' = M1 * M2
    bool operator==(const Matrix4& rhs) const;   // exact compare, no epsilon
    bool operator!=(const Matrix4& rhs) const;   // exact compare, no epsilon
    float operator[](int index) const;           // subscript operator v[0], v[1]
    float& operator[](int index);                // subscript operator v[0], v[1]

    friend Matrix4 operator-(const Matrix4& m); // unary operator (-)
    friend Matrix4 operator*(float scalar,
                             const Matrix4& m); // pre-multiplication
    friend Vector3 operator*(const Vector3& vec,
                             const Matrix4& m); // pre-multiplication
    friend Vector4 operator*(const Vector4& vec,
                             const Matrix4& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix4& m);

  protected:
  private:
    float get_cofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6,
                       float m7, float m8);

    float m[16];
};
#endif
