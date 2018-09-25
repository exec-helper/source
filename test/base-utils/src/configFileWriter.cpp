#include "configFileWriter.h"

#include <fstream>

using std::ofstream;
using std::string;

namespace execHelper {
namespace test {
namespace baseUtils {
ConfigFileWriter::ConfigFileWriter() noexcept
    : m_file("exec-helper.config.%%%%") {
    ;
}

Path ConfigFileWriter::getPath() const noexcept { return m_file.getPath(); }

string ConfigFileWriter::getFilename() const noexcept {
    return m_file.getFilename();
}

string ConfigFileWriter::getDirectory() const noexcept {
    return m_file.getParentDirectory();
}

bool ConfigFileWriter::write(const YamlWriter& yaml) noexcept {
    ofstream configStream;
    configStream.open(m_file.getPath(), ofstream::out | ofstream::trunc);
    configStream << yaml;
    configStream.close();
    return configStream.good();
}
} // namespace baseUtils
} // namespace test
} // namespace execHelper
