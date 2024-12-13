#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>

template<typename T>
class AbstractMatrix {
    public:
        AbstractMatrix& operator=(const AbstractMatrix& other);

        virtual ~AbstractMatrix() = default;

        virtual size_t get_n() const = 0;

        virtual size_t get_m() const = 0;

        void reset();

        virtual T* operator[](size_t i) = 0;
        virtual T* operator[](size_t i) const = 0;
};

template<typename T, size_t N, size_t M>
class StaticMatix : public AbstractMatrix<T> {
    private:
        std::unique_ptr<T[]> data;

    public:
        StaticMatix(size_t n, size_t m) : data(new T[N * M]{}){
            assert(n == N && m == M);
        }

        size_t get_n() const override {return N;}

        size_t get_m() const override {return M;}

        T* operator[](size_t i) override {return (T*)data.get() + i * M;}

        T* operator[](size_t i) const override {return (T*)data.get() + i * M;}
};

template<typename T>
class DynamicMatrix : public AbstractMatrix<T> {
    private:
        size_t N, M;;
        std::unique_ptr<T[]> data;
    
    public:
        DynamicMatrix(size_t N, size_t M)
          : data(new T[N * M]{}),
            N(N),
            M(M)
        {}

        T* operator[](size_t i) override {
            return (T*)data.get() + i * M;
        }

        T* operator[](size_t i) const override {
            return (T*)data.get() + i * M;
        }

        size_t get_n() const override {return N;}

        size_t get_m() const override {return M;}
};

struct size_marker {
    size_t n;
    size_t m;
};

template<typename T, size_marker... SZS>
class create_matrix_;

template<typename T, size_marker SZ, size_marker... SZS>
class create_matrix_<T, SZ, SZS...> {
public:
    AbstractMatrix<T>* operator()(size_t n, size_t m) {
        constexpr auto N = SZ.n;
        constexpr auto M = SZ.m;

        if (N == n && M == m) {
            return new StaticMatix<T, N, M>(N, M);
        }

        return create_matrix_<T, SZS...>{}(n, m);
    }
};

template<typename T>
class create_matrix_<T> {
public:
    AbstractMatrix<T>* operator()(size_t n, size_t m) {
        return new DynamicMatrix<T>(n, m);
    }
};

#define S(n, m) size_marker(n, m)

template<typename T>
struct create_matrix : public create_matrix_<T, SIZES> {};


template<typename T>
AbstractMatrix<T>& AbstractMatrix<T>::operator=(const AbstractMatrix<T>& other) {
    assert(get_n() == other.get_n() && get_m() == other.get_m());
    for (size_t i = 0; i < get_n(); ++i) {
        for (size_t j = 0; j < get_m(); ++j) {
            (*this)[i][j] = other[i][j];
        }
    }
    return *this;
}

template<typename T>
void AbstractMatrix<T>::reset() {
    for (size_t i = 0; i < get_n(); ++i) {
        for (size_t j = 0; j < get_m(); ++j) {
            (*this)[i][j] = T{};
        }
    }
}