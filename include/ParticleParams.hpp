#pragma once
#include "Const.hpp"

using namespace std;


template<typename PType, typename VType, typename VFType>
class Fluid;

template<typename PType, typename VType>
class ParticleParams {
public:
    char type;
    PType cur_p;
    array<VType, deltas.size()> v;

public:
    template<typename VFType>
    void swap_with(Fluid<PType, VType, VFType>& f, int x, int y);
};


template<typename PType, typename VType>
template<typename VFType>
void ParticleParams<PType, VType>::swap_with(Fluid<PType, VType, VFType>& f, int x, int y) {
    swap((*f.field)[x][y], type);
    swap((*f.p)[x][y], cur_p);
    swap((*f.velocity.v)[x][y], v);
}