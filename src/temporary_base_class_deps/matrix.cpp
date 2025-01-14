/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/14 17:09:38
# Desc   :
########################################################################
*/
#include "matrix.h"

#include <algorithm>

namespace matrix {
void add_to(const int* from, int* to, size_t dim) {
  for (size_t i = 0; i < dim * dim; ++i, ++from, ++to) {
    *to += *from;
  }
}
void init_matrix(int* data, size_t dim) {
  for (size_t i = 0; i < dim * dim; ++i) {
    data[i] = i * i;
  }
}
TMatrix::TMatrix(const TMatrix& tm): dim_(tm.dim_), data_(tm.data_) {
}
TMatrix::TMatrix(const Matrix& m): dim_(m.dim_), data_(new int[dim_ * dim_]) {
  std::copy(m.data_, m.data_ + dim_ * dim_, data_);
}
TMatrix& TMatrix::operator+(TMatrix tm) {
  add(tm);
  tm.release();
  return *this;
}
TMatrix& TMatrix::operator+(const Matrix& m) {
  add(m);
  return *this;
}
void TMatrix::add(const TMatrix& tm) {
  add_to(tm.data_, data_, dim_);
}
void TMatrix::release() {
  dim_ = 0;
  delete [] data_;
}

Matrix::Matrix(size_t dim): TMatrix() {
  dim_ = dim;
  data_ = new int[dim_ * dim];
  init_matrix(data_, dim_);
}
Matrix::Matrix(const Matrix& m): TMatrix(m) {}
Matrix::Matrix(const TMatrix& tm): TMatrix(tm) {}
Matrix& Matrix::operator=(const Matrix& m) {
  Matrix(m).swap(this);
  return *this;
}
Matrix& Matrix::operator=(const TMatrix& tm) {
  release();
  dim_ = tm.dim_;
  data_ = tm.data_;
  return *this;
}
TMatrix Matrix::operator+(const Matrix& m) {
  TMatrix tm(*this);
  tm.add(m);
  return tm;
}
TMatrix Matrix::operator+(const TMatrix& tm) {
  TMatrix tm2(tm);
  tm2.add(*this);
  return tm2;
}
Matrix::~Matrix() {
  delete [] data_;
}
void Matrix::swap(Matrix* m) {
  std::swap(dim_, m->dim_);
  std::swap(data_, m->data_);
}


}  // namespace matrix

