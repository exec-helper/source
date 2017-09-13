#include "tmpFile.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using boost::system::error_code;
using boost::system::errc::success;
using boost::filesystem::fstream;
using boost::filesystem::is_regular_file;
using boost::filesystem::remove;
using boost::filesystem::temp_directory_path;
using boost::filesystem::unique_path;

using execHelper::config::Path;

namespace execHelper {
namespace test {
namespace utils {
    TmpFile::TmpFile(const Path& model) :
        m_path(temp_directory_path() / unique_path(model))
    {
        ;
    }

    TmpFile::~TmpFile() noexcept {
        if(exists()) {
            remove(m_path); 
        }
    }

    bool TmpFile::exists() const noexcept {
        return is_regular_file(m_path);
    }

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

    config::Path TmpFile::getPath() const noexcept {
        return m_path;
    }

    std::string TmpFile::toString() const noexcept {
        return m_path.native();
    }

    std::string TmpFile::getFilename() const noexcept {
        return m_path.filename().native();
    }

    std::string TmpFile::getParentDirectory() const noexcept {
        return m_path.parent_path().native();
    }

    void TmpFile::clear() noexcept {
        m_path.clear();
    }
} // utils
} // test
} // execHelper
