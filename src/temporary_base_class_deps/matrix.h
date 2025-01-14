/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/14 16:24:58
# Desc   :
########################################################################
*/
#pragma once

#include <cstddef>

namespace matrix {

class Matrix;

class TMatrix {
public:
  ~TMatrix() = default;
  TMatrix& operator+(TMatrix m);
  TMatrix& operator+(const Matrix& tm);
protected:
  TMatrix() = default;
  TMatrix(const TMatrix& tm);
  TMatrix(const Matrix& m);

  void add(const TMatrix& tm);
  void release();
protected:
  size_t dim_{0};
  int* data_{nullptr};

  friend class Matrix;
};  // class TMatrix

class Matrix : public TMatrix {
public:
  explicit Matrix(size_t dim);
  Matrix(const Matrix& m);
  Matrix(const TMatrix& tm);
  Matrix& operator=(const Matrix& m);
  Matrix& operator=(const TMatrix& tm);
  TMatrix operator+(const Matrix& m);
  TMatrix operator+(const TMatrix& tm);
  ~Matrix();

  void swap(Matrix* m);
};  // class Matrix

}  // namespace matrix

