#ifndef CONFIG_FILE_WRITER_INCLUDE
#define CONFIG_FILE_WRITER_INCLUDE

#include <string>

#include "yaml.h"

#include "tmpFile.h"

namespace execHelper {
    namespace test {
        namespace utils {
            class ConfigFileWriter {
                public:
                    ConfigFileWriter() noexcept;

                    ConfigFileWriter(const ConfigFileWriter& other) = delete;
                    ConfigFileWriter(ConfigFileWriter&& other) noexcept = delete;

                    ConfigFileWriter& operator=(const ConfigFileWriter& other) = delete;
                    ConfigFileWriter& operator=(ConfigFileWriter&& other) noexcept = delete;

                    std::string getFilename() noexcept;
                    std::string getDirectory() noexcept;
                    bool write(const YamlWriter& yaml) noexcept;
            
                private:
                    TmpFile m_file;
            };
        } // namespace utils
    } // namespace test
} // namespace execHelper

#endif  /* CONFIG_FILE_WRITER_INCLUDE */
