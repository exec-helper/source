#include "configFileWriter.h"

#include <boost/filesystem/fstream.hpp>

using std::string;

using boost::filesystem::ofstream;

namespace execHelper {
    namespace test {
       namespace baseUtils {
            ConfigFileWriter::ConfigFileWriter() noexcept :
                m_file("exec-helper.config.%%%%")
            {
                ;
            }

            string ConfigFileWriter::getFilename() noexcept {
                return m_file.getFilename();
            }

            string ConfigFileWriter::getDirectory() noexcept {
                return m_file.getParentDirectory();
            }

            bool ConfigFileWriter::write(const YamlWriter& yaml) noexcept {
                ofstream configStream;
                configStream.open(m_file.getPath(), ofstream::out | ofstream::trunc);
                configStream << yaml;
                configStream.close();
                return configStream.good();
            }
       } // namespace utils 
    } // namespace test
} // namespace execHelper
