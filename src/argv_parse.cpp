#include "argv_parse.hpp"

#include <cstring>
#include <stdexcept>
#include <sstream>

using namespace std;

ArgvParseResult parseArgs(char** argv) {
    unordered_map<string, string> named;

    for (char** arg_ = argv + 1; *arg_ != nullptr; ++arg_) {
        string arg = *arg_;
        if (arg.starts_with("--")) {
            arg.erase(arg.begin(), arg.begin() + 2);
            auto eq_pos = arg.find('=');
            if (eq_pos == string::npos) {
                throw runtime_error("Не найден знак \"=\"");
            }
            string name(arg.begin(), arg.begin() + eq_pos);
            string value(arg.begin() + eq_pos + 1, arg.end());

            if (named.contains(name)) {
                throw runtime_error("Найдены повторяющиеся аргументы");
            }
            named.insert({move(name), move(value)});
        } else {
            throw runtime_error("Неверный аргумент");
        }
    }
    return ArgvParseResult(named);
}

const string& ArgvParseResult::get_option(const string& opt_name) {
    if (!named.contains(opt_name)) {
        if(opt_name == "savefile"){
            return "";
        }
        else{
            stringstream ss;
            ss << "Опция " << opt_name << " не найдена";
            throw runtime_error(ss.str());
        }
    }
    return named.at(opt_name);
}
