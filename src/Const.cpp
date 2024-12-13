#include "Const.hpp"

using namespace std;

mt19937 Rnd::rnd{1337};

string to_lower_rm_space(string_view s) {
    string ans;
    ans.reserve(s.size());
    for (char c : s) {
        if (isspace(c)) {
            continue;;
        }
        ans.push_back(tolower(c));
    }
    return ans;
}
