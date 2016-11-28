#include <cstdlib>

#include "core/execHelperOptions.h"
#include "core/posixShell.h"
#include "core/immediateExecutor.h"
#include "commander/commander.h"

using execHelper::core::ExecHelperOptions;
using execHelper::core::PosixShell;
using execHelper::core::ImmediateExecutor;
using execHelper::commander::Commander;

int execHelperMain(int argc, char** argv) {
    ExecHelperOptions options;
    options.parseSettingsFile(options.getSettingsFile(argc, argv));
    options.parse(argc, argv);

    PosixShell shell;
    ImmediateExecutor executor(shell);
    options.setExecutor(&executor);

    Commander commander(options);
    commander.run();

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    return execHelperMain(argc, argv);
}
