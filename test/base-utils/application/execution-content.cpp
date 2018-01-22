#include <exception>
#include <iostream>

#include "base-utils/commandUtils.h"
#include "base-utils/executionContent.h"
#include "base-utils/path.h"

using std::cerr;
using std::endl;
using std::exception;

using execHelper::test::baseUtils::ExecutionContentClient;
using execHelper::test::baseUtils::ExecutionContentData;
using execHelper::test::baseUtils::Path;
using execHelper::test::baseUtils::RUNTIME_ERROR;

int main(int argc, char** argv) {
    if(argc < 2) {
        cerr << "Insufficient arguments" << endl;
        return RUNTIME_ERROR;
    }

    ExecutionContentData data;
    // Ignore the first two arguments, as they are used for instructing this binary itself
    for(int i = 2; i < argc; ++i) {
        data.args.emplace_back(argv[i]);
    }

    Path endpoint(argv[1]);
    try {
        ExecutionContentClient client(endpoint);
        return client.addExecution(data);
    } catch(const exception& e) {
        cerr << "Caught exception: " << e.what() << endl;
        return RUNTIME_ERROR;
    }
}
