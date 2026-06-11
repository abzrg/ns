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


template<typename LeftExpr, typename RightExpr>
class AddExpr : public VecExpr<AddExpr<LeftExpr, RightExpr>>
{
    const LeftExpr& left_;
    const RightExpr& right_;

public:
    AddExpr(const LeftExpr& left, const RightExpr& right)
    :
        left_(left),
        right_(right)
    {}

    inline auto size() const
    {
        return left_.size();
    }

    inline auto operator[](size_t i) const
    {
        return left_[i] + right_[i];
    }
};
