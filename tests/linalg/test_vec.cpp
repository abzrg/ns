#include <iostream>
#include <complex>
#include <print>

#include <catch2/catch_test_macros.hpp>

#include <ns/linalg/vec.hpp>


// =================================================================================================
// Constructors
// =================================================================================================

TEST_CASE("Constructor: set size", "[vec][ctor]")
{
    SECTION("construct empty vector")
    {
        ns::Vec<int> w{};
        REQUIRE(w.size() == 0);
    }

    SECTION("construct by setting size")
    {
        ns::Vec<int> v(5);
        REQUIRE(v.size() == 5);
    }

    SECTION("construct by setting size and initial value")
    {
        ns::Vec v(5, 3.14);
        REQUIRE(v.size() == 5);
    }

    SECTION("construct via an initializer list")
    {
        ns::Vec<int> v{ 1, 2, 3 };
        REQUIRE(v.at(0) == 1);
        REQUIRE(v.at(1) == 2);
        REQUIRE(v.at(2) == 3);
    }
}

TEST_CASE("Constructor: initializer list", "[vec][access]") {}

// =================================================================================================
// Expression templates
// =================================================================================================

TEST_CASE("Nested expression", "[vec][expr]")
{
    ns::Vec<double> a{ 1, 2, 3 };
    ns::Vec<double> b{ 4, 5, 6 };
    ns::Vec<double> c{ 7, 8, 9 };

    ns::Vec<double> result = a + b + c;

    REQUIRE(result[0] == 12);
    REQUIRE(result[1] == 15);
    REQUIRE(result[2] == 18);
}

TEST_CASE("Expression assignment", "[vec][expr]")
{
    ns::Vec<double> a{ 1, 2, 3 };
    ns::Vec<double> b{ 4, 5, 6 };

    ns::Vec<double> result(3);

    result = a + b;

    REQUIRE(result == ns::Vec<double>{ 5, 7, 9 });
}

TEST_CASE("deep expression")
{
    ns::Vec<double> a{ 1, 1, 1 };

    ns::Vec<double> result = a + a + a + a + a + a;

    REQUIRE(result == ns::Vec<double>{ 6, 6, 6 });
}


// =================================================================================================
// Printing Vec (formatter and operator<<)
// =================================================================================================

TEST_CASE("Vec<int> formats correctly", "[vec][format]")
{
    SECTION("empty vector")
    {
        ns::Vec<int> v{};
        REQUIRE(std::format("{}", v) == "[]");
    }

    SECTION("single element")
    {
        ns::Vec<int> v{ 42 };
        REQUIRE(std::format("{}", v) == "[42]");
    }

    SECTION("multiple elements")
    {
        ns::Vec<int> v{ 1, 2, 3 };
        REQUIRE(std::format("{}", v) == "[1, 2, 3]");
    }
}

TEST_CASE("Vec<double> formats correctly", "[vec][format]")
{
    SECTION("simple values")
    {
        ns::Vec<double> v{ 1.5, 2.5, 3.5 };
        REQUIRE(std::format("{}", v) == "[1.5, 2.5, 3.5]");
    }

    SECTION("empty double vector")
    {
        ns::Vec<double> v{};
        REQUIRE(std::format("{}", v) == "[]");
    }
}

TEST_CASE("Vec format matches operator<< output", "[vec][format]")
{
    // Consistency check: std::format and operator<< must agree
    ns::Vec<int> v{ 10, 20, 30 };

    std::ostringstream oss;
    oss << v;

    REQUIRE(std::format("{}", v) == oss.str());
}

TEST_CASE("std::println compiles and runs without error", "[vec][format]")
{
    // Smoke test — just verify it compiles and doesn't throw
    ns::Vec<int> v{ 7, 8, 9 };
    REQUIRE_NOTHROW(std::println("{}", v));
}


// =================================================================================================
// operator== - integral types - exact comparison
// =================================================================================================

TEST_CASE("Vec<int> operator== exact comparison", "[vec][operator==][integral]")
{
    SECTION("equal vectors")
    {
        ns::Vec<int> a{ 1, 2, 3 };
        ns::Vec<int> b{ 1, 2, 3 };
        REQUIRE(a == b);
    }

    SECTION("unequal vectors")
    {
        ns::Vec<int> a{ 1, 2, 3 };
        ns::Vec<int> b{ 1, 2, 4 };
        REQUIRE_FALSE(a == b);
    }

    SECTION("differ by exactly 1 — must be unequal")
    {
        // Key property of exact comparison: no tolerance at all
        ns::Vec<int> a{ 10, 20, 30 };
        ns::Vec<int> b{ 10, 20, 31 };
        REQUIRE_FALSE(a == b);
    }

    SECTION("empty vectors are equal")
    {
        ns::Vec<int> a{};
        ns::Vec<int> b{};
        REQUIRE(a == b);
    }

    SECTION("single element")
    {
        REQUIRE(ns::Vec<int>{ 42 } == ns::Vec<int>{ 42 });
        REQUIRE_FALSE(ns::Vec<int>{ 42 } == ns::Vec<int>{ 43 });
    }

    SECTION("reflexivity: a == a")
    {
        ns::Vec<int> a{ 1, 2, 3 };
        REQUIRE(a == a);
    }

    SECTION("symmetry: a == b implies b == a")
    {
        ns::Vec<int> a{ 1, 2, 3 };
        ns::Vec<int> b{ 1, 2, 3 };
        REQUIRE(a == b);
        REQUIRE(b == a);
    }
}

TEST_CASE("Vec<long> operator== exact comparison", "[vec][operator==][integral]")
{
    SECTION("equal")
    {
        REQUIRE(ns::Vec<long>{ 1L, 2L } == ns::Vec<long>{ 1L, 2L });
    }
    SECTION("unequal")
    {
        REQUIRE_FALSE(ns::Vec<long>{ 1L, 2L } == ns::Vec<long>{ 1L, 3L });
    }
}

TEST_CASE("Vec<unsigned> operator== exact comparison", "[vec][operator==][integral]")
{
    SECTION("equal")
    {
        REQUIRE(ns::Vec<unsigned>{ 10u, 20u } == ns::Vec<unsigned>{ 10u, 20u });
    }
    SECTION("unequal")
    {
        REQUIRE_FALSE(ns::Vec<unsigned>{ 10u, 20u } == ns::Vec<unsigned>{ 10u, 21u });
    }
}

// =================================================================================================
// operator== - floating point types - approximate comparison
// =================================================================================================
//
// approx_equal defaults: rel_tol = 1e-9, abs_tol = 1e-12
//
// threshold for element x ≈ 1.0:
//   abs_tol + rel_tol * scale = 1e-12 + 1e-9 * 1.0 ≈ 1e-9
//
// So 1e-10 difference → equal, 1e-8 difference → unequal

TEST_CASE("Vec<double> operator== approximate comparison", "[vec][operator==][floating]")
{
    SECTION("exactly equal")
    {
        ns::Vec<double> a{ 1.0, 2.0, 3.0 };
        ns::Vec<double> b{ 1.0, 2.0, 3.0 };
        REQUIRE(a == b);
    }

    SECTION("difference within relative tolerance (1e-10 < 1e-9 threshold)")
    {
        ns::Vec<double> a{ 1.0, 2.0, 3.0 };
        ns::Vec<double> b{ 1.0 + 1e-10, 2.0 + 1e-10, 3.0 + 1e-10 };
        REQUIRE(a == b);
    }

    SECTION("difference outside relative tolerance (1e-8 > 1e-9 threshold)")
    {
        ns::Vec<double> a{ 1.0, 2.0, 3.0 };
        ns::Vec<double> b{ 1.0 + 1e-8, 2.0, 3.0 };
        REQUIRE_FALSE(a == b);
    }

    SECTION("only last element differs beyond tolerance")
    {
        ns::Vec<double> a{ 1.0, 2.0, 3.0 };
        ns::Vec<double> b{ 1.0, 2.0, 3.0 + 1e-8 };
        REQUIRE_FALSE(a == b);
    }

    SECTION("empty vectors are equal")
    {
        ns::Vec<double> a{};
        ns::Vec<double> b{};
        REQUIRE(a == b);
    }

    SECTION("single element within tolerance")
    {
        REQUIRE(ns::Vec<double>{ 1.0 } == ns::Vec<double>{ 1.0 + 1e-10 });
        REQUIRE_FALSE(ns::Vec<double>{ 1.0 } == ns::Vec<double>{ 1.0 + 1e-8 });
    }

    SECTION("reflexivity: a == a")
    {
        ns::Vec<double> a{ 1.5, 2.5, 3.5 };
        REQUIRE(a == a);
    }

    SECTION("symmetry: a == b implies b == a")
    {
        ns::Vec<double> a{ 1.0, 2.0 };
        ns::Vec<double> b{ 1.0 + 1e-10, 2.0 + 1e-10 };
        REQUIRE(a == b);
        REQUIRE(b == a);
    }

    // When both values are close to zero, rel_tol * scale vanishes and only abs_tol protects you
    // from a division-by-zero style failure in relative comparisons.
    SECTION("near-zero values use absolute tolerance")
    {
        // Both values near zero: scale ≈ 0, so threshold ≈ abs_tol = 1e-12
        ns::Vec<double> a{ 0.0 };
        ns::Vec<double> b{ 1e-13 };  // diff 1e-13 < abs_tol 1e-12 → equal
        REQUIRE(a == b);

        ns::Vec<double> c{ 1e-11 };  // diff 1e-11 > abs_tol 1e-12 → unequal
        REQUIRE_FALSE(a == c);
    }
}

TEST_CASE("Vec<float> operator== approximate comparison", "[vec][operator==][floating]")
{
    SECTION("equal")
    {
        REQUIRE(ns::Vec<float>{ 1.0f, 2.0f } == ns::Vec<float>{ 1.0f, 2.0f });
    }
    SECTION("clearly unequal")
    {
        REQUIRE_FALSE(ns::Vec<float>{ 1.0f } == ns::Vec<float>{ 2.0f });
    }
}

// =================================================================================================
// operator== - verify integral and floating-point behave differently
// =================================================================================================

TEST_CASE("Exact vs approximate: integral rejects what float accepts", "[vec][operator==]")
{
    // 1 + 1e-10 rounds to exactly 1 in int, but the point is:
    // for int, even a difference of 1 is rejected with no tolerance
    ns::Vec<int> ai{ 1000 };
    ns::Vec<int> bi{ 1001 };  // differs by 1 — unequal for int
    REQUIRE_FALSE(ai == bi);

    // For double, a difference of 1e-10 at scale ~1000 is within rel tolerance:
    // threshold = 1e-12 + 1e-9 * 1000 = ~1e-6, and 1e-10 < 1e-6
    ns::Vec<double> ad{ 1000.0 };
    ns::Vec<double> bd{ 1000.0 + 1e-10 };
    REQUIRE(ad == bd);
}

// int test_dot()
// {
//     return dot(Vec{1, 2, 3}, Vec{4, 5, 6}) == 32;
// }
//
// int test_hadamard()
// {
//     auto res = hadamard(Vec{1, 2, 3}, Vec{4, 5, 6});
//
//     assert(res[0] == 4);
//     assert(res[1] == 10);
//     assert(res[2] == 18);
//     return 0;
// }
//
// int test_inner()
// {
//     ns::Vec<int> a{1,2,3};
//     ns::Vec<int> b{4,5,6};
//
// }
//
// int test_access_boundcheck()
// {
//     ns::Vec<int> v(3);
//
//     try
//     {
//         v.at(100);
//         assert(false);
//     }
//     catch (...)
//     {
//         return 1;
//     }
//
//     return 0;
// }
//
// int test_complex_numbers()
// {
//     using cplx = std::complex<double>;
//
//     ns::Vec<cplx> a{{1,2},{3,4}};
//     ns::Vec<cplx> b{{5,6},{7,8}};
//
//     [[maybe_unused]] auto d = a*b;
//     // std::println("{}", d);
//     // python -c 'print((1+2j)*(5+6j) + (3+4j)*(7+8j))'
//     std::cout << d << std::endl;
//
//     auto inr1 = inner(a,b);
//     auto inr2 = std::conj(a.at(0))*b.at(0) + std::conj(a.at(1))*b.at(1);
//     std::cout << inr1 << std::endl;  // (70,-8)
//     std::cout << inr2 << std::endl;
// }
//
// int main()
// {
//     // Complex support
//
//     // Bounds safety test
//     {
//     }
// }
