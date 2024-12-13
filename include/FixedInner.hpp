#pragma once

#include <algorithm>
#include <ostream>
#include <istream>
#include <type_traits>
#include <climits>

template<typename T, size_t K>
requires (sizeof(T) * CHAR_BIT >= K)
class FixedInner {
public:
    using type = T;
    static constexpr size_t k = K;
    T v;

    constexpr FixedInner(int v): v(v << K) {}
    constexpr FixedInner(float f): v(f * (1 << K)) {}
    constexpr FixedInner(double f): v(f * (1 << K)) {}
    constexpr FixedInner(): v(0) {}

    constexpr operator double() const { return (double)v / (1 << K); }

    template<typename T2, size_t K2>
    constexpr FixedInner(const FixedInner<T2, K2>& other) {
        if      (K2 > K) {v = other.v >> (K2 - K);} 
        else if (K2 < K) {v = other.v << (K - K2);} 
        else             {v = other.v;}
    }

    static constexpr FixedInner from_raw(T x) {
        FixedInner ret;
        ret.v = x;
        return ret;
    } 

};

template<typename T1, typename T2>
struct CommonTypeFixed;

template<typename T1, size_t K1, typename T2, size_t K2>
struct CommonTypeFixed<FixedInner<T1, K1>, FixedInner<T2, K2>> {
    using type = FixedInner<typename std::common_type<T1, T2>::type, std::max(K1, K2)>;
};

template<typename T, size_t K, typename U>
requires std::is_arithmetic<U>::value
struct CommonTypeFixed<FixedInner<T, K>, U> {
    using type = FixedInner<T, K>;
};

template<typename T, size_t K, typename U>
requires std::is_arithmetic<U>::value
struct CommonTypeFixed<U, FixedInner<T, K>> {
    using type = FixedInner<T, K>;
};

template<typename T, typename U>
requires std::is_arithmetic<T>::value && std::is_arithmetic<U>::value 
struct CommonTypeFixed<T, U> {
    using type = typename std::common_type<T, U>::type;
};

template<typename T>
struct IsFixedInner {
    static constexpr bool value = false;
};

template<typename T, size_t K>
struct IsFixedInner<FixedInner<T, K>> {
    static constexpr bool value = true;
};

template<typename T1, typename T2>
struct IsEitherFixedInner {
    static constexpr bool value = IsFixedInner<T1>::value || IsFixedInner<T2>::value;
};

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
CommonTypeFixed<T1, T2>::type operator+(T1 lhs, T2 rhs) {
    using res_t = CommonTypeFixed<T1, T2>::type;
    return res_t::from_raw(res_t(lhs).v + res_t(rhs).v);
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
CommonTypeFixed<T1, T2>::type operator-(T1 lhs, T2 rhs) {
    using res_t = CommonTypeFixed<T1, T2>::type;
    return res_t::from_raw(res_t(lhs).v - res_t(rhs).v);
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
CommonTypeFixed<T1, T2>::type operator*(T1 lhs, T2 rhs) {
    using res_t = CommonTypeFixed<T1, T2>::type;
    return res_t::from_raw(((int64_t) res_t(lhs).v * res_t(rhs).v) >> res_t::k);
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
CommonTypeFixed<T1, T2>::type operator/(T1 lhs, T2 rhs) {
    using res_t = CommonTypeFixed<T1, T2>::type;
    return res_t::from_raw(((int64_t) res_t(lhs).v << res_t::k) / res_t(rhs).v);
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
T1& operator+=(T1& lhs, T2 rhs) {
    return lhs = lhs + (T1)rhs;
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
T1& operator-=(T1& lhs, T2 rhs) {
    return lhs = lhs - (T1)rhs;
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
T1& operator*=(T1& lhs, T2 rhs) {
    return lhs = lhs * (T1)rhs;
}

template<typename T1, typename T2>
requires IsEitherFixedInner<T1, T2>::value
T1& operator/=(T1& lhs, T2 rhs) {
    return lhs = lhs / (T1)rhs;
}

template<typename T, size_t K>
FixedInner<T, K> operator-(FixedInner<T, K> x) {
    return FixedInner<T, K>::from_raw(-x.v);
}

template<typename T, size_t K>
FixedInner<T, K> abs(FixedInner<T, K> x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}

template<typename T, size_t K>
std::ostream& operator<<(std::ostream& out, FixedInner<T, K> x) {
    return out << x.v / (double) (1 << K);
}

template<typename T, size_t K>
std::istream& operator>>(std::istream& in, FixedInner<T, K>& x) {
    double v;
    in >> v;
    x = FixedInner<T, K>(v);
    return in;
}
