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
#include "cpp_utils/util.h"


namespace matrix {
size_t TMatrix::kConstructFromMatrixCount{0};
size_t TMatrix::kCopyConstructCount{0};
size_t TMatrix::kReleaseCount{0};
size_t Matrix::kConstructCount{0};
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
  PRINT_CURRENT_FUNCTION_NAME;
  ++kCopyConstructCount;
}
TMatrix::TMatrix(const Matrix& m): dim_(m.dim_), data_(new int[dim_ * dim_]) {
  PRINT_CURRENT_FUNCTION_NAME;
  ++kConstructFromMatrixCount;
  std::copy(m.data_, m.data_ + dim_ * dim_, data_);
}
TMatrix& TMatrix::operator+(TMatrix tm) {
  PRINT_CURRENT_FUNCTION_NAME;
  add(tm);
  // 通过拷贝构造的方式将实参拷贝到实参，此时形参和实参共享 tm.data_，但是在 add(tm) 后，
  // tm.data_ 就可以被释放掉了
  tm.release();
  return *this;
}
TMatrix& TMatrix::operator+(const Matrix& m) {
  PRINT_CURRENT_FUNCTION_NAME;
  add(m);
  return *this;
}
void TMatrix::add(const TMatrix& tm) {
  add_to(tm.data_, data_, dim_);
}
void TMatrix::release() {
  dim_ = 0;
  delete [] data_;
  ++kReleaseCount;
}

void TMatrix::reset_count() {
  kConstructFromMatrixCount = 0;
  kCopyConstructCount = 0;
  kReleaseCount = 0;
}

Matrix::Matrix(size_t dim): TMatrix() {
  PRINT_CURRENT_FUNCTION_NAME;
  ++kConstructCount;
  dim_ = dim;
  data_ = new int[dim_ * dim];
  init_matrix(data_, dim_);
}
Matrix::Matrix(const Matrix& m): TMatrix(m) {}
Matrix::Matrix(const TMatrix& tm): TMatrix(tm) {}
Matrix& Matrix::operator=(const Matrix& m) {
  PRINT_CURRENT_FUNCTION_NAME;
  // copy m to this
  Matrix(m).swap(this);
  return *this;
}
Matrix& Matrix::operator=(const TMatrix& tm) {
  // share tm.data_ to this
  release();
  dim_ = tm.dim_;
  data_ = tm.data_;
  return *this;
}
TMatrix Matrix::operator+(const Matrix& m) {
  PRINT_CURRENT_FUNCTION_NAME;
  TMatrix tm(*this);
  tm.add(m);
  return tm;
}
TMatrix Matrix::operator+(const TMatrix& tm) {
  PRINT_CURRENT_FUNCTION_NAME;
  TMatrix tm2(tm);  // 让 tm2 和 tm 共享 tm.data_
  tm2.add(*this);
  return tm2;
}
Matrix::~Matrix() {
  release();
}
void Matrix::swap(Matrix* m) {
  std::swap(dim_, m->dim_);
  std::swap(data_, m->data_);
}

void Matrix::reset_count() {
  kConstructCount = 0;
}

}  // namespace matrix

