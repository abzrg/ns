#pragma once

#include <complex>
#include <concepts>


namespace ns
{

// Integer and floating point scalar

// note: some arithmetic expressions involving references/value categories may
// fail exact same_as<T> constraints. Also some user-defined numeric types
// return proxy objects. TODO: TEST
template<typename T>
concept LinearAlgebraScalar = requires(T a, T b) {
    { T{} };

    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a / b } -> std::same_as<T>;

    { a += b } -> std::same_as<T&>;
    { a -= b } -> std::same_as<T&>;
    { a *= b } -> std::same_as<T&>;
    { a /= b } -> std::same_as<T&>;
};


// Floating point scalar

template<typename T>
struct is_complex
:
    std::false_type
{};

template<typename T>
struct is_complex<std::complex<T>>
:
    std::true_type
{};

template<typename T>
concept FloatingLinearAlgebraScalar
    =
    (
        std::floating_point<T>
        || (is_complex<T>::value && std::floating_point<typename T::value_type>)
    )
    && LinearAlgebraScalar<T>;

}  // namespace ns
