#pragma once


#include <concepts>
#include <numbers>


namespace ns
{

template<std::floating_point Scalar>
constexpr Scalar pi{std::numbers::pi_v<Scalar>};

template<std::floating_point Scalar>
constexpr Scalar relative_tolerance{1e-9};

template<std::floating_point Scalar>
constexpr Scalar absolute_tolerance{1e-12};

}  // namespace ns
