#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct ArgvParseResult {
    const unordered_map<string, string> named;

    const string& get_option(const string& opt_name);
};

ArgvParseResult parseArgs(char** argv);
