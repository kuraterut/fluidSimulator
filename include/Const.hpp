#pragma once

#include <array>
#include <random>
#include <string>
#include <string_view>

using namespace std;

constexpr array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

string to_lower_rm_space(string_view s);

class Rnd {
private:
    static mt19937 rnd;
    
public:
    template<typename Type>
    static Type random01() {
        return Type(uniform_real_distribution<>{0, 1}(rnd));
    }
};



