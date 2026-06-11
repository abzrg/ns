#pragma once

#include <cassert>
#include <initializer_list>
#include <ostream>
#include <print>
#include <vector>
#include <format>
#include <sstream>

#include <ns/core.hpp>
#include <ns/linalg/vec_expr.hpp>


namespace ns
{

template<LinearAlgebraScalar Scalar>
class Vec : public VecExpr<Vec<Scalar>>
{
public:
    Vec() = default;

    explicit Vec(size_t sz)
    :
        elems_(sz)
    {}

    explicit Vec(size_t sz, Scalar initval)
    :
        elems_(sz, initval)
    {}

    // NOTE: initializer lists are usually passed by value because they are
    // basically lightweight proxy objects.
    Vec(std::initializer_list<Scalar> initlist)
    :
        elems_{ initlist }
    {}

    // Element-wise (Hadamard) product
    Vec<Scalar> hadamard(const Vec& other) const
    {
        assert(size() == other.size());

        Vec result(size());

        for (size_t i = 0, sz = size(); i < sz; ++i)
        {
            result.at(i) = at(i) * other.at(i);
        }

        return result;
    }

    // Construction from expressions
    template<typename Expr>
    Vec(const VecExpr<Expr>& expr);

    // Copy assignment from expressions
    template<typename Expr>
    Vec& operator=(const VecExpr<Expr>& expr);

    // ---

    // Inner product (Hermitian inner product)
    //
    // Over complex vector spaces, inner product usually means Hermitian inner
    // product
    //
    //     ⟨x,y⟩ = Σ_i \bar{x}_i y_i
    //
    Scalar inner(const Vec<Scalar>& other) const
    {
        assert(size() == other.size());

        Scalar result{};

        for (size_t i = 0, sz = size(); i < sz; ++i)
        {
            // NOTE: std::conj() promotes real numbers into complex numbers.
            // That's why we use the free function template `conjugate`.
            // It returns non-complex numbers as they are and via a
            // specialization returns complex conjugate of complex numbers.
            result += ns::conjugate(at(i)) * other.at(i);
        }

        return result;
    }

    inline Scalar operator*(const Vec& other) const
    {
        return inner(other);
    }

    // Algebraic dot product: dot(a, b)
    //
    // Over real vector spaces, dot product is the same as inner product
    //
    //     ⟨x,y⟩ = Σ_i x_i y_i
    //
    Scalar dot(const Vec& other) const
    {
        assert(size() == other.size());

        Scalar result{};

        for (size_t i = 0, sz = size(); i < sz; ++i)
        {
            result += at(i) * other.at(i);
        }

        return result;
    }

    Vec operator-(const Vec& other) const
    {
        assert(size() == other.size());

        Vec result(size());

        for (size_t i = 0, sz = size(); i < sz; ++i)
        {
            result.at(i) = at(i) - other.at(i);
        }

        return result;
    }

    Vec& operator-=(const Vec& other)
    {
        assert(size() == other.size());

        for (size_t i = 0, sz = size(); i < sz; ++i)
        {
            at(i) -= other.at(i);
        }

        return *this;
    }

    // Scalar multiplication
    Vec operator*(const Scalar& alpha) const
    {
        Vec result(size());

        for (size_t i = 0; i < size(); ++i)
        {
            result.at(i) = at(i) * alpha;
        }

        return result;
    }

    // Does not make sense
    auto operator*=(const Vec&) = delete;

    Vec& operator*=(const Scalar& alpha)
    {
        for (size_t i = 0; i < size(); ++i)
        {
            at(i) *= alpha;
        }

        return *this;
    }

    // Does not make sense
    auto operator/(const Vec&) const = delete;

    // Scalar division
    Vec operator/(const Scalar& alpha) const
    {
        auto inv = Scalar{ 1 } / alpha;
        return *this * inv;
    }

    Vec& operator/=(const Scalar& alpha)
    {
        for (size_t i = 0; i < size(); ++i)
        {
            at(i) /= alpha;
        }

        return *this;
    }

    // For now set to be deleted, as the base class 'VecExpr<Vec<int>>' hasn't defined it.
    // By default check equality for integers; specialize for the rest to use
    // approx_equal
    inline bool operator==(const Vec&) const;

    // L-2 norm
    inline auto norm() const
    {
        using std::abs;
        using std::sqrt;

        return sqrt(abs(inner(*this, *this)));
    }

    inline const Scalar& at(size_t index) const
    {
        checkindex(index);
        return elems_.at(index);
    }

    inline Scalar& at(size_t index)
    {
        checkindex(index);
        return elems_.at(index);
    }

    inline Scalar& operator[](size_t index)
    {
        return elems_[index];
    }

    inline const Scalar& operator[](size_t index) const
    {
        return elems_[index];
    }

    inline const std::vector<Scalar>& elems() const noexcept
    {
        return elems_;
    }

    inline size_t size() const noexcept
    {
        return elems_.size();
    }

    inline void resize(size_t new_size)
    {
        elems_.resize(new_size);
    }

    inline void checkindex(size_t index) const noexcept
    {
        assert(index < elems_.size());
    }

private:
    std::vector<Scalar> elems_{};
};



// Constructor taking vector expressions
template<LinearAlgebraScalar Scalar>
template<typename Expr>
Vec<Scalar>::Vec(const VecExpr<Expr>& expr)
{
    const Expr& e = expr.self();

    const auto sz = e.size();
    elems_.resize(sz);

    for (size_t i = 0; i < sz; ++i)
        elems_[i] = e[i];
}


// Copy assignment operator taking vector expression
template<LinearAlgebraScalar Scalar>
template<typename Expr>
Vec<Scalar>::Vec& Vec<Scalar>::operator=(const VecExpr<Expr>& expr)
{
    const Expr& e = expr.self();

    const auto sz = e.size();
    elems_.resize(sz);

    for (size_t i = 0; i < sz; ++i)
        elems_[i] = e[i];

    return *this;
}


// Equality check for floating point scalars
template<FloatingLinearAlgebraScalar FloatScalar>
bool approx_equal
(
    const Vec<FloatScalar>& vec,
    const Vec<FloatScalar>& other,
    f64 rel_tol = 1e-9,
    f64 abs_tol = 1e-12
)
{
    assert(vec.size() == other.size());

    for (size_t i = 0, sz = vec.size(); i < sz; ++i)
    {
        auto diff  = std::abs(vec.at(i) - other.at(i));
        auto scale = std::max(std::abs(vec.at(i)), std::abs(other.at(i)));
        if (diff > abs_tol + rel_tol * scale)
            return false;
    }

    return true;
}


// Equality operator that works both for integers and floats
template<LinearAlgebraScalar Scalar>
bool Vec<Scalar>::operator==(const Vec& vec) const
{
    if constexpr (std::integral<Scalar>)
    {
        // Exact comparison for integers
        return elems_ == vec.elems_;
    }
    else
    {
        return approx_equal(*this, vec);
    }
}


// Hermitian inner product
template<LinearAlgebraScalar Scalar>
Scalar inner(const Vec<Scalar>& vec, const Vec<Scalar>& other)
{
    return vec.inner(other);
}


// Algebraic dot product (different from Hermitian inner product)
template<LinearAlgebraScalar Scalar>
Scalar dot(const Vec<Scalar>& vec, const Vec<Scalar>& other)
{
    assert(vec.size() == other.size());

    Scalar result{};

    for (size_t i = 0, sz = vec.size(); i < sz; ++i)
    {
        result += vec.at(i) * other.at(i);
    }

    return result;
}


// L-2 norm
template<LinearAlgebraScalar Scalar>
auto norm(const Vec<Scalar>& vec)
{
    return vec.norm();
}


// Component-wise multiplication
template<LinearAlgebraScalar Scalar>
Vec<Scalar> hadamard(const Vec<Scalar>& vec, const Vec<Scalar>& other)
{
    return vec.hadamard(other);
}


// Scalar-vector multiplication
template<LinearAlgebraScalar Scalar>
Vec<Scalar> operator*(const Scalar& alpha, const Vec<Scalar>& vec)
{
    return vec * alpha;
}


// Addition of vector expressions
template<typename LeftExpr, typename RightExpr>
auto operator+(const VecExpr<LeftExpr>& left_expr, const VecExpr<RightExpr>& right_expr)
{
    return AddExpr<LeftExpr, RightExpr>(left_expr.self(), right_expr.self());
}


// Left-shift operator for printing a vector
template<LinearAlgebraScalar Scalar>
std::ostream& operator<<(std::ostream& os, const Vec<Scalar>& vec)
{
    os << "[";

    if (vec.size() == 0)
    {
        os << "]";
        return os;
    }

    for (size_t i = 0; i < vec.size() - 1; i++)
    {
        os << vec.at(i) << ", ";
    }

    os << vec.at(vec.size() - 1) << "]";

    return os;
}


}  // namespace ns



// Partial specialization of formatter for Vec
template<ns::LinearAlgebraScalar Scalar>
struct std::formatter<ns::Vec<Scalar>>
{
    // No custom format spec parsing for now
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const ns::Vec<Scalar>& vec, std::format_context& ctx) const
    {
        std::ostringstream oss;
        oss << vec;
        return std::format_to(ctx.out(), "{}", oss.str());
    }
};
