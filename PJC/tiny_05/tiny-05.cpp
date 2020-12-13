#include "tiny-05.hpp"

namespace pjc {

complex::complex(double real, double imaginary) :
    m_real(real),
    m_imag(imaginary) {}

double complex::real() const {
    return m_real;
}

void complex::real(double d) {
    m_real = d;
}

double complex::imag() const {
    return m_imag;
}

void complex::imag(double d) {
    m_imag = d;
}

complex complex::operator+(const complex& rhs) const {
    return complex(this->real() + rhs.real(), this->imag() + rhs.imag());
}

complex complex::operator+(const double& rhs) const {
    return complex(this->real() + rhs, this->imag());
}

complex operator+(const double& lhs, const complex& rhs) {
    return complex(lhs + rhs.real(), rhs.imag());
}

complex complex::operator-(const complex& rhs) const {
    return complex(this->real() - rhs.real(), this->imag() - rhs.imag());
}

complex complex::operator-(const double& rhs) const {
    return complex(this->real() - rhs, this->imag());
}

complex operator-(const double& lhs, const complex& rhs) {
    return complex(lhs - rhs.real(), - rhs.imag());
}

complex complex::operator*(const complex& rhs) const {
    return complex(this->real() * rhs.real() - this->imag() * rhs.imag(), this->real() * rhs.imag() + this->imag() * rhs.real());
}

complex complex::operator*(const double& rhs) const {
    return complex(this->real() * rhs, this->imag() * rhs);
}

complex operator*(const double& lhs, const complex& rhs) {
    return complex(lhs * rhs.real(), lhs * rhs.imag());
}
}
