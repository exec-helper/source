#include <cstdlib>

#include "core/execHelperOptions.h"

using execHelper::core::ExecHelperOptions;

int execHelperMain(int argc, char** argv) {
    ExecHelperOptions options;
    options.parseSettingsFile(options.getSettingsFile(argc, argv));
    options.parse(argc, argv);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    return execHelperMain(argc, argv);
}
