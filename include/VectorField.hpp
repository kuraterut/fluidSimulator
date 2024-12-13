#pragma once

#include "Matrix.hpp"
#include "Const.hpp"

#include <array>
#include <algorithm>
#include <cassert>
#include <memory>

using namespace std;

template<typename T>
class VectorField {
public:
    unique_ptr<AbstractMatrix<array<T, deltas.size()>>> v = nullptr;

public:
    VectorField(){v = nullptr;}
    VectorField(size_t N, size_t M) : v(create_matrix<array<T, deltas.size()>>{}(N, M)){}

    void reset();

    T& add(int x, int y, int dx, int dy, T dv);

    T& get(int x, int y, int dx, int dy);
};


template<typename T>
void VectorField<T>::reset(){
    v->reset();
}

template<typename T>
T& VectorField<T>::add(int x, int y, int dx, int dy, T dv){
    assert(v.get() != nullptr);
    return get(x, y, dx, dy) += dv;
}

template<typename T>
T& VectorField<T>::get(int x, int y, int dx, int dy) {
    assert(v.get() != nullptr);
    size_t i = ranges::find(deltas, make_pair(dx, dy)) - deltas.begin();
    assert(i < deltas.size());
    return (*v)[x][y][i];
}