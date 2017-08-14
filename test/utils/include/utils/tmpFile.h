#ifndef TMP_FILE_INCLUDE
#define TMP_FILE_INCLUDE

#include "config/path.h"

namespace execHelper {
    namespace test {
        namespace utils {
            class TmpFile {
                public:
                    TmpFile() noexcept;
                    TmpFile(const std::string& filename) noexcept;
                    TmpFile(const config::Path& directory) noexcept;
                    ~TmpFile() noexcept;

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
