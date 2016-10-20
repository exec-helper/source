#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <fstream>

bool fileExists(const std::string& file) {
    std::ifstream infile(file.c_str());
    return infile.good();
}

void writeToFile(const std::string& file, const std::string& content) {
    uint32_t numberOfRuns = 1U;

    if(fileExists(file)) {
        std::fstream readFile(file.c_str(), std::ios_base::in);
        readFile >> numberOfRuns;
        ++numberOfRuns;
    }

    std::ofstream outputFile;
    outputFile.open(file.c_str());
    outputFile << numberOfRuns << std::endl;
    outputFile.close();
}

#endif // __COMMON_H__
