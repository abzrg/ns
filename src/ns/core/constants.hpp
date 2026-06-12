#pragma once


#include <concepts>
#include <numbers>

#include <ns/core/type_aliases.hpp>


namespace ns
{

template<std::floating_point Scalar>
constexpr Scalar pi{std::numbers::pi_v<Scalar>};

inline constexpr f64 relative_tolerance = 1e-9;
inline constexpr f64 absolute_tolerance = 1e-12;

}  // namespace ns
