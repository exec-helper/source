#include <cstdlib>

#include "core/execHelperOptions.h"

using execHelper::ExecHelperOptions;

int main(int argc, char** argv) {
    ExecHelperOptions options;
    options.parse(argc, argv);
    return EXIT_SUCCESS;
}
