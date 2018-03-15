#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "src/layout.h"
#include "src/util.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, const char **argv) {
    try {
        po::variables_map vm;

        // common options
        po::options_description options("CLI and ~/.xLayoutDisplay");
        options.add_options()
                ("mirror,m", "mirror outputs using the lowest common resolution")
                ("order,o", po::value<vector<string>>(), "order of outputs, repeat as needed")
                ("primary,p", po::value<string>(), "primary output")
                ("quiet,q", "suppress feedback");

        // file options
        po::options_description fileOptions("~/.xLayoutDisplay");
        fileOptions.add(options);

        // command line options
        po::options_description cliOptions("CLI");
        cliOptions.add_options()
                ("help,h", "print this help text and exit")
                ("info,i", "print information about current outputs and exit")
                ("noop,n", "perform a trial run and exit");
        cliOptions.add(options);

        // command line options take precedence
        po::store(po::command_line_parser(argc, argv).options(cliOptions).run(), vm);
        po::notify(vm);

        // file options afterwards
        ifstream ifs(resolveTildePath(".xLayoutDisplay"));
        if (ifs) {
            po::store(parse_config_file(ifs, fileOptions), vm);
            po::notify(vm);
        }

        // usage
        if (vm.count("help")) {
            cout << "Arranges outputs in a left to right manner, using highest resolution and refresh." << endl;
            cout << "DPI is calculated exactly based on the first or primary output's EDID information." << endl;
            cout << "Laptop outputs are turned off when the lid is closed." << endl;
            cout << endl;
            cout << "e.g.  xLayoutDisplay -p DP-4 -o HDMI-0 -o DP-4" << endl;
            cout << endl;
            cout << cliOptions << endl;
            return EXIT_SUCCESS;
        }

        // render settings
        const Settings settings(vm);

        // execute
        return WEXITSTATUS(layout(settings));
    } catch (const exception &e) {
        fprintf(stderr, "FAIL: %s, exiting\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        fprintf(stderr, "EPIC FAIL: unknown exception, exiting\n");
        return EXIT_FAILURE;
    }
}
