#pragma once

#include <complex>

#include <ns/core/concepts.hpp>


namespace ns
{

// Conjugation of scalars. Handles non-complex numbers by returning them without
// change.
template<LinearAlgebraScalar Scalar>
constexpr Scalar conjugate(const Scalar& x)
{
    return x;
}

// And, actually returns the conjugate of complex numbers if the input is one.
template<LinearAlgebraScalar Scalar>
constexpr std::complex<Scalar> conjugate(const std::complex<Scalar>& x)
{
    return std::conj(x);
}

}  // namespace ns
