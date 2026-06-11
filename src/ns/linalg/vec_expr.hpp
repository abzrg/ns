#pragma once

//
// Expression-template overview
// ----------------------------
//
// Traditional vector addition:
//
//     Vec c = a + b + d;
//
// typically performs:
//
//     tmp1 = a + b
//     tmp2 = tmp1 + d
//     c    = tmp2
//
// creating temporary vectors and copying intermediate results.
//
// Expression templates instead build a compile-time expression tree:
//
//            AddExpr
//           /       \
//      AddExpr       d
//      /     \
//     a       b
//
// No arithmetic is performed while building the tree.
//
// Evaluation happens only when constructing or assigning a concrete
// vector, allowing the entire expression to be computed in a single pass:
//
//     for (i)
//         c[i] = a[i] + b[i] + d[i];
//

//
// This file contains
// - concepts for vector expressions
// - storage policy
// - CRTP base vector expression
// - AddExpr expression operator
//

//
// Later file split:
//
// linalg/
//     vec_expr.hpp          // concepts + CRTP + storage
//     vec_add_expr.hpp
//     vec_sub_expr.hpp
//     vec_scalar_expr.hpp
//


#include <concepts>

#include <ns/core/type_aliases.hpp>
#include <ns/core/concepts.hpp>


namespace ns
{


// --- VectorExpression Concept


// Concept satisfied by concrete vectors and expression-template nodes.
template<typename Vector>
concept VectorExpression = requires(const Vector& vec, size_t i)
{
    typename std::remove_cvref_t<Vector>::expression_tag;
    typename std::remove_cvref_t<Vector>::value_type;

    { vec.size() } -> std::convertible_to<size_t>;
    vec[i];
};


// --- VecExpr Storage


// Forward Declaration
template<LinearAlgebraScalar Scalar>
class Vec;


// Expression nodes must decide how to store their operands.
//
// Consider:
//
//     auto expr = a + b;
//
// If AddExpr stores its operands by reference, no copies occur:
//
//     AddExpr
//       ├─&a
//       └─&b
//
// This is desirable because vectors may be large.
//
// However, now consider:
//
//     auto expr = a + b + c;
//
// The compiler first creates a temporary AddExpr(a,b).
// The outer AddExpr then receives:
//
//     AddExpr( temporary_AddExpr , c )
//
// If expression nodes were also stored by reference, the outer AddExpr
// would contain a reference to the temporary inner AddExpr. Once the
// full expression is built, that temporary disappears and a dangling
// reference remains.
//
// Storing everything by value fixes the lifetime problem but causes
// expensive copies of concrete vectors.
//
// The solution is:
//
//     Vec          -> store by const reference
//     Expression   -> store by value
//
// Expression nodes are typically tiny objects containing only references
// or other expression nodes, so copying them is cheap. Concrete vectors
// own data and should not be copied unnecessarily.
//
// This policy eliminates both dangling references and redundant vector
// copies.
template<typename T>
struct expr_storage
{
    // Store expression nodes by value by default.
    using type = T;
};


template<LinearAlgebraScalar Scalar>
struct expr_storage<Vec<Scalar>>
{
    // Concrete vectors are stored by reference to avoid copies.
    using type = const Vec<Scalar>&;
};


template<typename T>
using expr_storage_t = typename expr_storage<std::remove_cvref_t<T>>::type;



// --- VecExpr and Vector-Generating Operations (Add, Sub, ScalarMul, Hadamard)


// Base class for all vector expressions using the Curiously Recurring
// Template Pattern (CRTP).
//
// Every vector-like object derives from VecExpr:
//
//     Vec<double>
//     AddExpr<...>
//     SubExpr<...>
//     ...
//
// This allows algorithms and operators to accept any vector expression
// through the common VecExpr interface without using virtual functions.
//
// Example:
//
//     Vec<double> a, b, c;
//
//     auto expr = a + b + c;
//
// The resulting type is approximately:
//
//     AddExpr
//     <
//         AddExpr<Vec<double>, Vec<double>>,
//         Vec<double>
//     >
//
// All nodes in the expression tree derive from VecExpr, allowing generic
// code to treat them uniformly.
template<typename Derived>
struct VecExpr
{
    // A marker-tag which is just declared to help with the definition of a concept for vector
    // expressions.
    using expression_tag = void;

    // Return the most-derived object.
    //
    // VecExpr itself does not store any data. Instead it forwards operations to the actual derived
    // type using CRTP.
    //
    // Example:
    //
    //     AddExpr expr(...);
    //
    // Inside VecExpr<AddExpr>, self() returns a reference to expr.
    inline const Derived& self() const
    {
        return static_cast<const Derived&>(*this);
    }

    // Forward size() calls to the derived expression.
    //
    // This allows generic code to query the size of any vector expression
    // through the VecExpr interface.
    inline auto size() const
    {
        return self().size();
    }

    // Forward element access to the derived expression.
    //
    // Expression nodes compute values lazily. Calling expr[i] evaluates only
    // the i-th element of the expression.
    inline auto operator[](size_t i) const
    {
        return self()[i];
    }
};


// Lazy vector-addition expression.
//
// This class represents:
//
//     left_expr + right_expr
//
// without immediately computing any values.
//
// Example:
//
//     auto expr = a + b;
//
// stores references to a and b and computes elements on demand:
//
//     expr[i] == a[i] + b[i]
//
// More complex expressions naturally form trees:
//
//     a + b + c
//
// becomes:
//
//            AddExpr
//           /       \
//      AddExpr       c
//      /     \
//     a       b
//
// Evaluation occurs only when a concrete Vec is constructed or assigned
// from the expression.
template<VectorExpression LeftExpr, VectorExpression RightExpr>
class AddExpr : public VecExpr<AddExpr<LeftExpr, RightExpr>>
{
    expr_storage_t<LeftExpr> left_expr_;
    expr_storage_t<RightExpr> right_expr_;
public:

    // Scalar type produced by this expression.
    //
    // The operands may have different scalar types:
    //
    //     Vec<float>  a;
    //     Vec<double> b;
    //
    // The result of a + b should behave like double arithmetic.
    //
    // std::common_type_t computes the scalar type that can represent values
    // from both operands.
    using value_type =
        std::common_type_t
        <
             typename LeftExpr::value_type,
             typename RightExpr::value_type
        >;

    AddExpr(const LeftExpr& left_expr, const RightExpr& right_expr)
    :
        left_expr_(left_expr),
        right_expr_(right_expr)
    {
         assert(left_expr.size() == right_expr.size());
    }

    inline auto size() const
    {
        return left_expr_.size();
    }

    inline auto operator[](size_t i) const
    {
        return left_expr_[i] + right_expr_[i];
    }

};


}  // namespace ns
