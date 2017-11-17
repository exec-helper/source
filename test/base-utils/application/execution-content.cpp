#include <iostream>
#include <exception>

#include "base-utils/commandUtils.h"
#include "base-utils/executionContent.h"
#include "base-utils/path.h"

using std::cerr;
using std::endl;
using std::exception;

using execHelper::test::baseUtils::ExecutionContentClient;
using execHelper::test::baseUtils::Path;
using execHelper::test::baseUtils::RUNTIME_ERROR;

int main(int argc, char** argv) {
    if(argc != 2) {
        cerr << "Insufficient arguments" << endl;
        return RUNTIME_ERROR;
    }

    Path endpoint(argv[1]);
    try {
        ExecutionContentClient client(endpoint);
        return client.addExecution();
    } catch(const exception& e) {
        cerr << "Caught exception: " << e.what() << endl;
        return RUNTIME_ERROR;
    }
}
