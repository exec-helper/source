#ifndef TMP_FILE_INCLUDE
#define TMP_FILE_INCLUDE

#include "config/path.h"

namespace execHelper {
    namespace test {
        namespace utils {
            class TmpFile {
                public:
                    TmpFile() noexcept;
                    explicit TmpFile(const std::string& filename) noexcept;
                    explicit TmpFile(const config::Path& directory) noexcept;
                    
                    TmpFile(const TmpFile& other) = delete;
                    TmpFile(TmpFile&& other) noexcept = delete;
                    ~TmpFile() noexcept;

                    TmpFile& operator=(const TmpFile& other) = delete;
                    TmpFile& operator=(TmpFile&& other) noexcept = delete;

                    bool exists() const noexcept;
                    bool create(const std::string& content = "") const noexcept;
                    bool createDirectories() const noexcept;

                    config::Path getPath() const noexcept;
                    std::string toString() const noexcept;
                    std::string getFilename() const noexcept;
                    std::string getParentDirectory() const noexcept;

                private:
                    config::Path m_path;
            };
            
        } // utils
        
    } // test
    
} // execHelper

#endif  /* TMP_FILE_INCLUDE */
