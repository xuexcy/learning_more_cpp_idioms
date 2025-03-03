/**
将多个密切相关的代数抽象（数字）隐藏在单个泛型抽象之后，并为其提供泛型接口。

使用场景: 只想使用抽象基类，隐藏子类细节

Number 是抽象基类，Complex 和 RealNumber 实际存储了数据的继承子类
1. 在 Number 中存储子类指针 Number* ref, 通过 ref 来操作子类,读取子类数据
2. 将数据运算方法在 Number 设置为 virtual, 由子类实现具体运算逻辑，通过虚函数在 Number 层面进行计算

Number n1 = Number::makeComplex(1, 2);
Number n2 = Number::makeReal(10);
Number n3 = n1 + n2;
Number n2 = n3;

*/

// #include <expected>
#include <optional>
#include <print>
#include <string>

#include "utils/main_decorator.h"

struct BaseConstructor {
    BaseConstructor(int = 0) {}
};

class RealNumber;
class Complex;
class Number;

class Number {
    friend class RealNumber;
    friend class Complex;

public:
    static Number makeReal(double r);
    static Number makeComplex(double rpart, double ipart);
protected:
    Number();
    // 用于初始化继承类的 reference_count
    Number(BaseConstructor);
public:
    Number(const Number& n);
    virtual ~Number();
public:
    Number& operator=(const Number& n);
    virtual Number operator+(const Number& n) const;
    std::optional<std::string> string() const;
    void swap(Number& n) throw ();
protected:
    virtual std::string to_string() const { return ""; }
private:
    void redefine(Number* n);
    virtual Number complexAdd(const Complex& n) const;
    virtual Number realAdd(const RealNumber& n) const;

private:
    // idiom: handle body
    Number* rep{nullptr};  // 指向一个继承类指针
    short referenceCount{0};
};

class Complex : public Number {
    friend class RealNumber;
    friend class Number;
public:
    Complex(double d, double e);
    Complex(const Complex& c);
    virtual ~Complex();
public:
    virtual Number operator+(const Number& n) const override;
    virtual Number realAdd(const RealNumber& n) const override;
    virtual Number complexAdd(const Complex& n) const override;
    virtual std::string to_string() const override;
public:
    double rpart, ipart;
};

class RealNumber : public Number {
    friend class Complex;
    friend class Number;
private:
    RealNumber(double r);
    RealNumber(const RealNumber& r);
    virtual ~RealNumber();
private:
    virtual Number operator+(const Number& n) const override;
    virtual Number realAdd(const RealNumber& n) const override;
    virtual Number complexAdd(const Complex& n) const override;
    virtual std::string to_string() const override;
private:
    double val;
};

Number::Number(BaseConstructor): rep(0), referenceCount(1) {
}

Number::Number(): rep(0), referenceCount(0) {
}

Number::Number(const Number& n): rep(n.rep), referenceCount(0) {
    std::print("Constructing a number using Number::Number\n");
    if (n.rep) {
        ++n.rep->referenceCount;
    }
}

Number Number::makeReal(double r) {
    Number n;
    n.redefine(new RealNumber(r));
    return n;
}

Number Number::makeComplex(double rpart, double ipart) {
    Number n;
    n.redefine(new Complex(rpart, ipart));
    return n;
}

Number::~Number() {
    if (rep && --rep->referenceCount == 0) {
        delete rep;
    }
}
Number& Number::operator=(const Number& n) {
    std::print("Assigning a Number using Number::operator=\n");
    Number temp(n);
    this->swap(temp);
    return *this;
}

void Number::swap(Number& n) throw() {
    std::swap(this->rep, n.rep);
}

Number Number::operator+(const Number& n) const {
    return rep->operator+(n);
}
// std::expected<std::string, std::string> Number::string() const {
//     if (nullptr == rep) {
//         return std::unexpected("Number not init");
//     }
//     return rep->to_string();
// }
std::optional<std::string> Number::string() const {
    if (nullptr == rep) {
        return std::nullopt;
    }
    return rep->to_string();
}

Number Number::complexAdd(const Complex& n) const {
    return rep->complexAdd(n);
}

Number Number::realAdd(const RealNumber& n) const {
    return rep->realAdd(n);
}

void Number::redefine(Number* n) {
    if (rep && --rep->referenceCount == 0) {
        delete rep;
    }
    rep = n;
}

Complex::Complex(double d, double e):
    Number(BaseConstructor()),
    rpart((d)), ipart(e) {

    std::print("Constructing a Complex\n");
}

Complex::Complex(const Complex& c):
    Number(BaseConstructor()),
    rpart(c.rpart), ipart(c.ipart) {

    std::print("Constructing a Complex using Complex::Complex\n");
}

Complex::~Complex() {
    std::print("Inside Complex::~Complex()\n");
}

Number Complex::operator+(const Number& n) const {
    return n.complexAdd(*this);
}

Number Complex::realAdd(const RealNumber& n) const {
    std::print("Complex::realAdd\n");
    return Number::makeComplex(this->rpart + n.val, this->ipart);
}

Number Complex::complexAdd(const Complex& n) const {
    std::print("Complex::complexAdd\n");
    return Number::makeComplex(this->rpart + n.rpart, this->ipart + n.ipart);
}

std::string Complex::to_string() const {
    return std::format("Complex{{real_part: {}, imaginary_part: {}}}", rpart, ipart);
}

RealNumber::RealNumber(double r): Number(BaseConstructor()), val(r) {
    std::print("Constructing a RealNumber\n");
}

RealNumber::RealNumber(const RealNumber& r): Number(BaseConstructor()), val(r.val) {
    std::print("Constructing a RealNumber using RealNumber::RealNumber\n");
}

RealNumber::~RealNumber() {
    std::print("Inside RealNumber::~RealNumber()\n");
}

Number RealNumber::operator+(const Number& n) const {
    return n.realAdd(*this);
}

Number RealNumber::realAdd(const RealNumber& n) const {
    return Number::makeReal(this->val + n.val);
}

Number RealNumber::complexAdd(const Complex& n) const {
    return Number::makeComplex(this->val + n.rpart, n.ipart);
}

std::string RealNumber::to_string() const {
    return std::format("RealNumber{{{}}}", val);
}

template <>
void std::swap(Number& n1, Number& n2) {
    n1.swap(n2);
}

// void print(const std::string& name, const Number& n) {
//     auto s = n.string();
//     if (s.has_value()) {
//         std::print("{}: {}\n", name, s.value());
//     } else {
//         std::print("{}: error: {}\n", name, s.error());
//     }
// }
void print(const std::string& name, const Number& n) {
    std::print("{}: {}\n", name, n.string().value_or("null"));
}
#define PRINT(name) print(#name, name)

int main() {
    utils::MainDecorator::Access();
    Number n1 = Number::makeComplex(1, 2);
    PRINT(n1);
    Number n2 = Number::makeReal(10);
    PRINT(n2);
    Number n3 = n1 + n2;
    PRINT(n3);
    n3 = n2;
    PRINT(n3);

    return 0;
}
