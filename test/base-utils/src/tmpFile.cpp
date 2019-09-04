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
inline Path unique_path(const Path& model) {
    string resultPathName = model.filename().string();
    auto nbOfReplacements =
        count(resultPathName.begin(), resultPathName.end(), '%');
    auto replacements = generateRandomChar(nbOfReplacements);
    size_t pos = 0U;
    auto index = 0U;

    while((pos = resultPathName.find('%', pos)) != std::string::npos) {
        assert(index < replacements.size());
        resultPathName.replace(pos, 1U, string(1U, replacements[index]));
        ++pos;
        ++index;
    }
    return resultPathName;
}
} // namespace

namespace execHelper {
namespace test {
namespace baseUtils {
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
} // namespace baseUtils
} // namespace test
} // namespace execHelper
