#include "Simulator.hpp"


int main(int argc, char** argv) {
    ArgvParseResult opts = parseArgs(argv);

    const string PType      = opts.get_option("p-type");
    const string VType      = opts.get_option("v-type");
    const string VFType     = opts.get_option("v-flow-type");
    const string filename   = opts.get_option("file");

    using types = type_list<TYPES>;
    using types_product = product<types, types, types>::type;

    bool need_save;
    const string save_filename = opts.get_option("savefile");
    if(save_filename == ""){need_save = false;}
    else {need_save = true;}

    Simulator sim{filename, need_save, save_filename};
    bool impl_found = run_for_matching<Simulator, types_product>{}(sim, {PType, VType, VFType});

    if (!impl_found) {cerr << "Типы не найдены" << endl;}
}
