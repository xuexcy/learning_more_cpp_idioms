/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/11 15:02:21
# Desc   :
########################################################################
*/

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <print>

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"
#include "temporary_base_class_deps/matrix.h"

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
namespace bad_case {
class Matrix {
private:
  size_t dim_{0};
  int* data_{nullptr};
public:
  static size_t kConstructCount;
  static size_t kCopyConstructCount;
  explicit Matrix(size_t d): dim_(d), data_(new int[d * d]()) {
    PRINT_CURRENT_FUNCTION_NAME;
    ++kConstructCount;
    init_matrix(data_, dim_);
  }
  Matrix(const Matrix& m) : dim_(m.dim_), data_(new int[dim_ * dim_]) {
    PRINT_CURRENT_FUNCTION_NAME;
    ++kCopyConstructCount;
    std::copy(m.data_, m.data_ + dim_ * dim_, data_);
  }
  Matrix& operator=(const Matrix& m) {
    Matrix(m).swap(*this);
    return *this;
  }
  Matrix operator+(const Matrix& m) const {
    Matrix result(m);
    add_to(this->data_, result.data_, dim_);
    return result;
  }
  ~Matrix() {
    delete [] data_;
  }
private:
  void swap(Matrix& m) {
    std::swap(dim_, m.dim_);
    std::swap(data_, m.data_);
  }
};  // class Matrix
size_t Matrix::kConstructCount{0};
size_t Matrix::kCopyConstructCount{0};

/**
根据 stdout 可以看到， Matrix 构造了 7 次
local variable:
  1. Matrix m1;
  2. Matrix m2;
  3. Matrix m3;
  4. Matrix m4;
temporary variable:
  1. Matrix temp1 = m1 + m2;
  2. Matrix temp2 = temp1 + m3;
  2. Matrix temp3 = temp2 + m4;
另外，如果没有 rvo，那么 Matrix result = temp3; 也需要发生一次构造
@stdout:
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
bad_case::Matrix::Matrix(size_t)
Matrix::kConstructCount: 7
Matrix::kCopyConstructCount: 0
*/
void run() {
  Matrix m1(3);
  Matrix m2(3);
  Matrix m3(3);
  Matrix m4(3);
  Matrix result = m1 + m2 + m3 + m4;
  std::println("Matrix::kConstructCount: {}", Matrix::kConstructCount);
  std::println("Matrix::kCopyConstructCount: {}", Matrix::kCopyConstructCount);
}
}  // namespace bad_case

/**
根据在 bad_case::Matrix 中遇到的问题，我们可以看到，表达式 m1 + m2 + m3 + m4 新建了三个临时变量，其中除了最后一个
temp3 通过 rvo 的方式赋值给了 result 外，另外两个 temp1、temp2 都在表达式结束后立刻析构掉了，对于 temp1、temp2，当 dim
很大时，构造和析构都会比较浪费资源(构造耗时、浪费内存)，但是，实际上我们只需要构造一个 temp1，并将后续的计算结果都写到
temp1 里，并最终通过 rvo 的方式将 temp1 的结果给到 result，即:
  temp1 = m1 + m2;
  temp1 = temp1 + m3;
  temp1 = temp1 + m4;
  result = tem1;  // rvo
这样，我们所需的构造次数就是 5 次，即 m1、m2、m3、m4、result(tmp1)，也就是 lvalue 的个数

根据上面的分析，我们定义一个 TMatrix 用于存储临时值，定义一个 class Matrix 用于
存储给用户使用的值。实现该方案的关键在于，将表达式的计算结果都存储于 TMatrix， 表达式计算结果后在将结果赋值给 Matrix。
根据下面 TMatrix 和 Matrix 的定义可以看到:
1. 用于表达式计算的操作符 operator+ 的返回结果都是 TMatrix，即将计算的临时结果存入 TMatrix
2. 按照 wiki 中的描述, TMatrix 的 operator+() 返回的是 *this，最好

construct cases:
  1. TMatrix tm1(dim): 不存在
  2. TMatrix tm1(tm2): 在编译器由 rvo 时，不需要 copy constructor
  3. TMatrix tm1(m1):  a. new tm1.data_   b. copy m1.data_ to tm1.data_
  4. Matrix  m1(dim):  a. new m1.data_    b. init m1.data_
  5. Matrix  m1(m2):   a. new m1.data_    b. copy m2.data_ to m1.data_
  6. Matrix  m1(tm1):  swap(m1.data_, tm1.data_)
operator cases:
  1. tm1 + tm2  :    a. add_to(&tm2, &tm1)  b. free(tm2)    c. return tm1
  2. tm1 + m2   :    a. add_to(&m2, &tm1)   b. return tm1
  3. m1  + m2   :    a. TMatrix tm1(m1)        b. add_to(&m2, &tm1);    c. return tm1
  4. m1  + tm2  :    a. add_to(&m1, &tm2, &tm2)   b. return tm2
assignment cases:
  1. tm1 = m1:   不存在
  2. tm1 = tm2:  不存在
  3. m1  = tm1:  swap(m1.data_, tm1.data_)
  4. m1  = m2:   a. free(m1)  b. new m1.data_  c. copy m2.data_ to m1.data_


对 namespace solution 进行分析
class TMatrix 分析:
  1. 对于表达式中有 tm 的，所有计算结果都写入到 tm 中，比如 tm1 + tm2, tm1 + m1, m1 + tm1
  2. 如果表达式这没有 tm，即 m1 + m2，此时需要先用 m1 构造出 tm1 ，再计算 tm1 + m2
根据以上两点可知:
  1. TMatrix 只有一个构造函数 TMatrix(const Matrix&)，并且只在 m1 + m2 时发生，也就是在
    Matrix::operator+(const Matrix& m) 中，所以 TMatrix 的这个构造函数是私有的，只能由 friend class Matrix
    调用
  2. 由于 tm1 + tm2 和 tm1 + m1，其结算逻辑都相同，所以 TMatrix 只有一个 operator+(const MatrixBase&)，不过
    这样就需要 MatrixBase 有一个虚函数

class Matrix 分析:
  1. 构造:
    a. 普通构造 Matrix(size_t dim)
    b. 拷贝构造 Matrix(const Matrix& m): 在本 idioms 中我们是想要优化临时变量，下方的表达式中并没有使用拷贝构造，
      所以根据 stdout 我们可以看到 Matrix::kCopyConstructCount 是 0
    c. 由 TMatrix 构造，由于 TMatrix 是一个临时变量，如果用于构造 Matrix，那把它所持有的数据 move 给 Matrix 即可，即
      m.swap(&tm)
  2. 赋值:
    a. 拷贝赋值操作符: 通过 Matrix(m).swap(this) 来避免原有的 this->data_ 内存忘记被释放
    b. 由 TMatrix 赋值，由于 TMatrix 是一个临时变量，如果赋值给 Matrix, 那把它所持有的数据 move 给 Matrix 即可，
      另外为了防止原有的 this->data_ 内存忘记被释放，也需用 swap 的方式来实现，即 tm.swap(this);
  3. operator:
    a. m1 + m2: TMatrix(m1) + m2 以得到一个 TMatrix
    b. m1 + tm1: tm1 + m2 以得到一个 TMatrix
*/
namespace solution {
class Matrix;
class TMatrix;

class MatrixBase {
public:
  static size_t kReleaseCount;
  static size_t kDestructCount;
  static void reset_count() {
    kReleaseCount = 0;
    kDestructCount = 0;
  }
protected:
  MatrixBase() = default;
  MatrixBase(const MatrixBase& rhs): dim_(rhs.dim_), data_(new int[dim_ * dim_]) {
    std::copy(rhs.data_, rhs.data_ + rhs.dim_ * rhs.dim_, data_);
    // std::println("{} -> {}", (void*)(&rhs), (void*)this);
  }
  void add(const MatrixBase& other) {
    add_to(other.data_, data_, dim_);
  }
  void swap(MatrixBase* other) {
    std::swap(dim_, other->dim_);
    std::swap(data_, other->data_);

  }
  ~MatrixBase() {
    if (nullptr != data_) {
      delete [] data_;
      ++kReleaseCount;
    }
    ++kDestructCount;
  }
  size_t dim_{0};
  int* data_{nullptr};
  friend class Matrix;
  friend class TMatrix;
};
size_t MatrixBase::kDestructCount{0};
size_t MatrixBase::kReleaseCount{0};

class TMatrix : public MatrixBase {  // temporary matrix
private:
  TMatrix() = default;
  TMatrix(const Matrix& m);
public:
  static size_t kConstructFromMatrixCount;
  static void reset_count() {
    kConstructFromMatrixCount = 0;
  }
  TMatrix operator+(const MatrixBase& tm);
  friend class Matrix;
};  // class TMatrix

class Matrix : public MatrixBase {
public:
  static size_t kConstructCount;
  static size_t kConstructFromTMatrixCount;
  static size_t kCopyConstructCount;
  static void reset_count() {
    kConstructCount = 0;
    kConstructFromTMatrixCount = 0;
    kCopyConstructCount = 0;
  }
  explicit Matrix(size_t dim);
  Matrix(const Matrix& m);
  Matrix(TMatrix tm);
  Matrix& operator=(const Matrix& m);
  Matrix& operator=(TMatrix tm);
  TMatrix operator+(const Matrix& m) const;  // 这是一个 const 函数
  TMatrix operator+(TMatrix tm) const;  // 这是一个 const 函数
  friend class TMatrix;
};  // class Matrix

size_t TMatrix::kConstructFromMatrixCount{0};
// 在 Matrix::operator+(const Matrix& m) 中被调用，即 m1 + m2 时，将其变成 TMatrix(m1) + m2
TMatrix::TMatrix(const Matrix& m): MatrixBase(m) {
  PRINT_CURRENT_FUNCTION_NAME;
  ++kConstructFromMatrixCount;
}

// tm1 + m1
// tm1 + tm2
TMatrix TMatrix::operator+(const MatrixBase& mb) {
  PRINT_CURRENT_FUNCTION_NAME;
  // std::println("{} + {}", (void*)this, (void*)(&mb));
  this->add(mb);
  TMatrix res;
  res.swap(this);
  return res;
}

size_t Matrix::kConstructCount{0};
size_t Matrix::kCopyConstructCount{0};
size_t Matrix::kConstructFromTMatrixCount{0};
Matrix::Matrix(size_t dim) {
  dim_ = dim;
  data_ = new int[dim_ * dim_];
  init_matrix(data_, dim_);
  PRINT_CURRENT_FUNCTION_NAME;
  ++kConstructCount;
}

// Matrix m1(m2);
Matrix::Matrix(const Matrix& m): MatrixBase(m) {
  PRINT_CURRENT_FUNCTION_NAME;
  ++kCopyConstructCount;
}
// Matrix m1 = tm1;
Matrix::Matrix(TMatrix tm) : MatrixBase() {
  PRINT_CURRENT_FUNCTION_NAME;
  ++kConstructFromTMatrixCount;
  tm.swap(this);
}
// m1 = m2;
Matrix& Matrix::operator=(const Matrix& m) {
  Matrix(m).swap(this);
  return *this;
}
// m1 = tm1;
Matrix& Matrix::operator=(TMatrix tm) {
  tm.swap(this);
  return *this;
}

// m1 + m2
TMatrix Matrix::operator+(const Matrix&  m) const {
  PRINT_CURRENT_FUNCTION_NAME;
  TMatrix tm(*this);
  tm.add(*this);
  return tm;  // nrvo
}
// m1 + tm1
TMatrix Matrix::operator+(TMatrix tm) const {
  tm.add(*this);
  return tm;  // nrvo
}

void run() {
  std::println();
  {
    Matrix m1(3);
    Matrix m2(3);
    Matrix m3(3);
    Matrix m4(3);
    // std::println("1: {}, 2: {}, 3: {}, 4: {}", (void*)(&m1), (void*)(&m2), (void*)(&m3), (void*)(&m4));
    Matrix result = m1 + m2 + m3 + m4;
    std::println("TMatrix::kConstructFromMatrixCount: {}", TMatrix::kConstructFromMatrixCount);
    std::println("Matrix::kConstructCount: {}", Matrix::kConstructCount);
    std::println("Matrix::kCopyConstructCount: {}", Matrix::kCopyConstructCount);
    std::println("Matrix::kConstructFromTMatrixCount: {}", Matrix::kConstructFromTMatrixCount);
  }
  std::println("MatrixBase::kDestructCount: {}", MatrixBase::kDestructCount);
  std::println("MatrixBase::kReleaseCount: {}", MatrixBase::kReleaseCount);
  std::println();
  MatrixBase::reset_count();
  TMatrix::reset_count();
  Matrix::reset_count();
  {
    Matrix m1(3);
    Matrix m2(3);
    Matrix m3(3);
    Matrix m4(3);
    // std::println("1: {}, 2: {}, 3: {}, 4: {}", (void*)(&m1), (void*)(&m2), (void*)(&m3), (void*)(&m4));
    // 根据 stdout 可以看到，m3 + m4 也生成了一个临时变量
    Matrix result = m1 + m2 + (m3 + m4);
    std::println("TMatrix::kConstructFromMatrixCount: {}", TMatrix::kConstructFromMatrixCount);
    std::println("Matrix::kConstructCount: {}", Matrix::kConstructCount);
    std::println("Matrix::kCopyConstructCount: {}", Matrix::kCopyConstructCount);
    std::println("Matrix::kConstructFromTMatrixCount: {}", Matrix::kConstructFromTMatrixCount);
  }
  std::println("MatrixBase::kDestructCount: {}", MatrixBase::kDestructCount);
  std::println("MatrixBase::kReleaseCount: {}", MatrixBase::kReleaseCount);
}

}  // namespace solution


namespace matrix {
void run() {
  std::println();
  {
    Matrix m1(3);
    Matrix m2(3);
    Matrix m3(3);
    Matrix m4(3);
    // std::println("1: {}, 2: {}, 3: {}, 4: {}", (void*)(&m1), (void*)(&m2), (void*)(&m3), (void*)(&m4));
    Matrix result = m1 + m2 + m3 + m4;
    //std::println("TMatrix::kConstructFromMatrixCount: {}", TMatrix::kConstructFromMatrixCount);
    //std::println("Matrix::kConstructCount: {}", Matrix::kConstructCount);
    //std::println("Matrix::kCopyConstructCount: {}", Matrix::kCopyConstructCount);
    //std::println("Matrix::kConstructFromTMatrixCount: {}", Matrix::kConstructFromTMatrixCount);
  }
  //std::println("MatrixBase::kDestructCount: {}", MatrixBase::kDestructCount);
  //std::println("MatrixBase::kReleaseCount: {}", MatrixBase::kReleaseCount);
  //std::println();
  //MatrixBase::reset_count();
  //TMatrix::reset_count();
  //Matrix::reset_count();
  {
    Matrix m1(3);
    Matrix m2(3);
    Matrix m3(3);
    Matrix m4(3);
    // std::println("1: {}, 2: {}, 3: {}, 4: {}", (void*)(&m1), (void*)(&m2), (void*)(&m3), (void*)(&m4));
    // 根据 stdout 可以看到，m3 + m4 也生成了一个临时变量
    Matrix result = m1 + m2 + (m3 + m4);
    //std::println("TMatrix::kConstructFromMatrixCount: {}", TMatrix::kConstructFromMatrixCount);
    //std::println("Matrix::kConstructCount: {}", Matrix::kConstructCount);
    //std::println("Matrix::kCopyConstructCount: {}", Matrix::kCopyConstructCount);
    //std::println("Matrix::kConstructFromTMatrixCount: {}", Matrix::kConstructFromTMatrixCount);
  }
  //std::println("MatrixBase::kDestructCount: {}", MatrixBase::kDestructCount);
  //std::println("MatrixBase::kReleaseCount: {}", MatrixBase::kReleaseCount);
}
}

int main() {
  utils::MainDecorator::Access();
  bad_case::run();
  solution::run();
  matrix::run();
  return 0;
}
