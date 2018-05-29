#include "tmpFile.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "base-utils/generateRandom.h"

using std::count;
using std::string;

using boost::filesystem::fstream;
using boost::filesystem::is_regular_file;
using boost::filesystem::remove;
using boost::filesystem::temp_directory_path;
using boost::system::error_code;
using boost::system::errc::success;

using execHelper::test::baseUtils::generateRandomChar;
using execHelper::test::baseUtils::Path;

namespace {
/**
 * Since valgrind reports boost::filesyste::unique_path to leak, we implement
 * our own version
 */
inline Path unique_path(Path model) {
    string resultPathName = model.filename().native();
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
    : m_path(temp_directory_path() / unique_path(model)) {
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
    return (error == success);
}

Path TmpFile::getPath() const noexcept { return m_path; }

std::string TmpFile::toString() const noexcept { return m_path.native(); }

std::string TmpFile::getFilename() const noexcept {
    return m_path.filename().native();
}

std::string TmpFile::getParentDirectory() const noexcept {
    return m_path.parent_path().native();
}

void TmpFile::clear() noexcept { m_path.clear(); }
} // namespace baseUtils
} // namespace test
} // namespace execHelper
