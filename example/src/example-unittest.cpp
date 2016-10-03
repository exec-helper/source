#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>

#include "common.h"

using std::cout;
using std::endl;
using std::string;
using std::strncpy;

int main(int /*argc*/, char** /*argv*/) {
    cout << "Execute unittest" << endl;

    char fullPath[sizeof(__FILE__)];
    strncpy(fullPath, __FILE__, sizeof(__FILE__));
    string filename(string(basename(fullPath)) + ".output");
    writeToFile(filename, "Unittest!");


    return EXIT_SUCCESS;
}
