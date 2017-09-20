#ifndef TMP_FILE_INCLUDE
#define TMP_FILE_INCLUDE

#include "path.h"

namespace execHelper {
    namespace test {
        namespace baseUtils {
            class TmpFile {
                public:
                    explicit TmpFile(const Path& model = "exec-helper.tmp.%%%%");
                    TmpFile(const TmpFile& other) = default;
                    TmpFile(TmpFile&& other) = default;     // NOLINT(misc-noexcept-move-constructor)
                    ~TmpFile() noexcept;

                    TmpFile& operator=(const TmpFile& other) = default;
                    TmpFile& operator=(TmpFile&& other) = default;  // NOLINT(misc-noexcept-move-constructor)

                    bool exists() const noexcept;
                    bool create(const std::string& content = "") const noexcept;
                    bool createDirectories() const noexcept;
                    void clear() noexcept;

                    Path getPath() const noexcept;
                    std::string toString() const noexcept;
                    std::string getFilename() const noexcept;
                    std::string getParentDirectory() const noexcept;

                private:
                    Path m_path;
            };
        } // namespace utils
    } // namespace test
} // namespace execHelper

#endif  /* TMP_FILE_INCLUDE */
