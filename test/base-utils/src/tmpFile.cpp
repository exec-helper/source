#include "tmpFile.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

#include "base-utils/generateRandom.h"

using std::count;
using std::error_code;
using std::fstream;
using std::string;

using execHelper::test::baseUtils::generateRandomChar;
using execHelper::test::baseUtils::Path;

namespace filesystem = std::filesystem;

namespace {
/**
 * Since valgrind reports boost::filesystem::unique_path to leak, we implement
 * our own version
 */
inline auto unique_path(string model) -> Path {
    auto nbOfReplacements =
        count(model.begin(), model.end(), '%');
    auto replacements = generateRandomChar(nbOfReplacements);
    size_t pos = 0U;
    auto index = 0U;

    while((pos = model.find('%', pos)) != std::string::npos && index < replacements.size()) {
        model[pos] = replacements[index];
        ++pos;
        ++index;
    }
    return model;
}
} // namespace

namespace execHelper::test::baseUtils {
TmpFile::TmpFile(const string& model)
    : m_path(filesystem::temp_directory_path() / ::unique_path(model)) {
    ;
}

TmpFile::~TmpFile() noexcept {
    if(exists()) {
        remove(m_path);
    }
}

bool TmpFile::exists() const noexcept { return is_regular_file(m_path); }

bool TmpFile::create(const std::string& content) const noexcept {
    createDirectories();

    fstream file;
    file.open(m_path, fstream::out | fstream::trunc);
    file << content;
    file.close();
    return true;
}

bool TmpFile::createDirectories() const noexcept {
    error_code error;
    create_directories(m_path.parent_path(), error);
    return error.operator bool();
}

Path TmpFile::getPath() const noexcept { return m_path; }

std::string TmpFile::toString() const noexcept { return m_path.string(); }

std::string TmpFile::getFilename() const noexcept {
    return m_path.filename().string();
}

std::string TmpFile::getParentDirectory() const noexcept {
    return m_path.parent_path().string();
}

void TmpFile::clear() noexcept { m_path.clear(); }
} // namespace execHelper::test::baseUtils
