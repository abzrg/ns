#pragma once

#include <complex>

#include <ns/core/concepts.hpp>


namespace ns
{

// Conjugation of scalars. Handles non-complex numbers by returning them without
// change.
template<LinearAlgebraScalar U>
constexpr U conjugate(const U& x)
{
    return x;
}

// And, actually returns the conjugate of complex numbers if the input is one.
template<LinearAlgebraScalar U>
constexpr std::complex<U> conjugate(const std::complex<U>& x)
{
    return std::conj(x);
}

}  // namespace ns
