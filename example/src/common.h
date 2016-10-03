#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <fstream>

void writeToFile(const std::string& file, const std::string& content) {
    std::ofstream outputFile;
    outputFile.open(file);
    outputFile << content << std::endl;
    outputFile.close();
}

#endif // __COMMON_H__
