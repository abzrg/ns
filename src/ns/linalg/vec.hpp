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

template<LinearAlgebraScalar S>
class Vec : public VecExpr<Vec<S>>
{
public:
    Vec() = default;

    explicit Vec(size_t sz)
    : elems_(sz)
    {
    }

    explicit Vec(size_t sz, S initval)
    : elems_(sz, initval)
    {
    }

    // NOTE: initializer lists are usually passed by value because they are
    // basically lightweight proxy objects.
    Vec(std::initializer_list<S> initlist)
    : elems_{ initlist }
    {
    }

    // Element-wise (Hadamard) product
    Vec<S> hadamard(const Vec& other) const
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
    S inner(const Vec<S>& other) const
    {
        assert(size() == other.size());

        S result{};

        for (size_t i = 0; i < size(); ++i)
        {
            // NOTE: std::conj() promotes real numbers into complex numbers.
            // That's why we use the free function template `conjugate`.
            // It returns non-complex numbers as they are and via a
            // specialization returns complex conjugate of complex numbers.
            result += ns::conjugate(at(i)) * other.at(i);
        }

        return result;
    }

    inline S operator*(const Vec& other) const
    {
        return inner(other);
    }

    // Algebraic dot product: dot(a, b)
    //
    // Over real vector spaces, dot product is the same as inner product
    //
    //     ⟨x,y⟩ = Σ_i x_i y_i
    //
    S dot(const Vec& other) const
    {
        assert(size() == other.size());

        S result{};

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

        for (size_t i = 0; i < size(); ++i)
        {
            result.at(i) = at(i) - other.at(i);
        }

        return result;
    }

    Vec& operator-=(const Vec& other)
    {
        assert(size() == other.size());

        for (size_t i = 0; i < size(); ++i)
        {
            at(i) -= other.at(i);
        }

        return *this;
    }

    // Scalar multiplication
    Vec operator*(const S& alpha) const
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

    Vec& operator*=(const S& alpha)
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
    Vec operator/(const S& alpha) const
    {
        auto inv = S{ 1 } / alpha;
        return *this * inv;
    }

    Vec& operator/=(const S& alpha)
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

    inline const S& at(size_t index) const
    {
        checkindex(index);
        return elems_.at(index);
    }

    inline S& at(size_t index)
    {
        checkindex(index);
        return elems_.at(index);
    }

    inline S& operator[](size_t index)
    {
        return elems_[index];
    }

    inline const S& operator[](size_t index) const
    {
        return elems_[index];
    }

    inline const std::vector<S>& elems() const noexcept
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
    std::vector<S> elems_{};
};


// --- Free functions


template<FloatingLinearAlgebraScalar FS>
bool approx_equal(const Vec<FS>& vec, const Vec<FS>& other, f64 rel_tol = 1e-9, f64 abs_tol = 1e-12)
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


template<LinearAlgebraScalar S>
S dot(const Vec<S>& vec, const Vec<S>& other)
{
    assert(vec.size() == other.size());

    S result{};

    for (size_t i = 0, sz = vec.size(); i < sz; ++i)
    {
        result += vec.at(i) * other.at(i);
    }

    return result;
}


template<LinearAlgebraScalar S>
S inner(const Vec<S>& vec, const Vec<S>& other)
{
    return vec.inner(other);
}


template<LinearAlgebraScalar S>
auto norm(const Vec<S>& vec)
{
    return vec.norm();
}


template<LinearAlgebraScalar S>
Vec<S> hadamard(const Vec<S>& vec, const Vec<S>& other)
{
    return vec.hadamard(other);
}


template<LinearAlgebraScalar S>
Vec<S> operator*(const S& alpha, const Vec<S>& vec)
{
    return vec * alpha;
}


template<LinearAlgebraScalar S>
std::ostream& operator<<(std::ostream& os, const Vec<S>& vec)
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


template<typename LHS, typename RHS>
auto operator+(const VecExpr<LHS>& lhs, const VecExpr<RHS>& rhs)
{
    return AddExpr<LHS, RHS>(lhs.self(), rhs.self());
}


template<LinearAlgebraScalar S>
template<typename Expr>
Vec<S>::Vec(const VecExpr<Expr>& expr)
{
    const Expr& e = expr.self();

    elems_.resize(e.size());

    for (size_t i = 0; i < e.size(); ++i)
        elems_[i] = e[i];
}


template<LinearAlgebraScalar S>
template<typename Expr>
Vec<S>::Vec& Vec<S>::operator=(const VecExpr<Expr>& expr)
{
    const Expr& e = expr.self();

    elems_.resize(e.size());

    for (size_t i = 0; i < e.size(); ++i)
        elems_[i] = e[i];

    return *this;
}


template<LinearAlgebraScalar S>
bool Vec<S>::operator==(const Vec& vec) const
{
    if constexpr (std::integral<S>)
        // Exact comparison
        return elems_ == vec.elems_;
    else
        return approx_equal(*this, vec);
}

}  // namespace ns


// Partial specialization of formatter for Vec
template<ns::LinearAlgebraScalar S>
struct std::formatter<ns::Vec<S>>
{
    // No custom format spec parsing for now
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const ns::Vec<S>& vec, std::format_context& ctx) const
    {
        std::ostringstream oss;
        oss << vec;
        return std::format_to(ctx.out(), "{}", oss.str());
    }
};
