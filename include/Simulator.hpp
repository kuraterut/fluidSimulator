#include "Fluid.hpp"
#include "Fixed.hpp"
#include "FastFixed.hpp"
#include "argv_parse.hpp"
#include "type_list.hpp"
#include "type_marker.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace std;

struct Simulator {
    string filename;
    bool need_save;
    string save_filename;

    template<typename P_TYPE, typename V_TYPE, typename V_FLOW_TYPE>
    void execute() {
        Fluid<P_TYPE, V_TYPE, V_FLOW_TYPE> fluid(filename, need_save, save_filename);
        fluid.run();
    }
};


struct double_type_marker {
    using type = double;

    bool matches(string_view s) const {
        return to_lower_rm_space(s) == "double";
    }  
};

struct float_type_marker {
    using type = float;

    bool matches(string_view s) const {
        return to_lower_rm_space(s) == "float";
    }  
};

template<size_t N, size_t K>
struct fixed_type_marker {
    using type = Fixed<N, K>;

    bool matches(string_view s) const {
        stringstream ss;
        ss << "fixed(" << N << "," << K << ")";
        return to_lower_rm_space(s) == ss.str();
    }  
};

template<size_t N, size_t K>
struct fast_fixed_type_marker {
    using type = FastFixed<N, K>;

    bool matches(string_view s) const {
        stringstream ss;
        ss << "fast_fixed(" << N << "," << K << ")";
        return to_lower_rm_space(s) == ss.str();
    }  
};

#define FLOAT float_type_marker
#define DOUBLE double_type_marker
#define FIXED(N, K) fixed_type_marker<N, K>
#define FAST_FIXED(N, K) fast_fixed_type_marker<N, K>
