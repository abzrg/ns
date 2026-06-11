#pragma once

#include <ns/core/type_aliases.hpp>


template<typename Derived>
struct VecExpr
{
    inline const Derived& self() const
    {
        return static_cast<const Derived&>(*this);
    }

    inline auto size() const
    {
        return self().size();
    }

    inline auto operator[](size_t i) const
    {
        return self()[i];
    }
};


template<typename LHS, typename RHS>
class AddExpr : public VecExpr<AddExpr<LHS, RHS>>
{
    const LHS& lhs_;
    const RHS& rhs_;

public:
    AddExpr(const LHS& lhs, const RHS& rhs)
    :
        lhs_(lhs),
        rhs_(rhs)
    {}

    inline auto size() const
    {
        return lhs_.size();
    }

    inline auto operator[](size_t i) const
    {
        return lhs_[i] + rhs_[i];
    }
};
