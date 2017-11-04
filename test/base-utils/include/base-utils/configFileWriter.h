#ifndef CONFIG_FILE_WRITER_INCLUDE
#define CONFIG_FILE_WRITER_INCLUDE

#include <string>

#include "path.h"
#include "tmpFile.h"
#include "yaml.h"

namespace execHelper {
namespace test {
namespace baseUtils {
class ConfigFileWriter {
  public:
    ConfigFileWriter() noexcept;
    ~ConfigFileWriter() = default;

    ConfigFileWriter(const ConfigFileWriter& other) = delete;
    ConfigFileWriter(ConfigFileWriter&& other) noexcept = delete;

    ConfigFileWriter& operator=(const ConfigFileWriter& other) = delete;
    ConfigFileWriter& operator=(ConfigFileWriter&& other) noexcept = delete;

    Path getPath() const noexcept;
    std::string getFilename() const noexcept;
    std::string getDirectory() const noexcept;
    bool write(const YamlWriter& yaml) noexcept;

  private:
    TmpFile m_file;
};
} // namespace baseUtils
} // namespace test
} // namespace execHelper

#endif /* CONFIG_FILE_WRITER_INCLUDE */
