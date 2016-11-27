#ifndef __CONFIG_INPUT_FILE_H__
#define __CONFIG_INPUT_FILE_H__

#include <string>

#include "config/settingsNode.h"

namespace execHelper {
    namespace config {
        // Interface for configuration input files
        class ConfigInputFile {
            public:
                virtual void getTree(const std::initializer_list<std::string>& keys, SettingsNode& settings) const noexcept = 0;

            protected:
                ConfigInputFile() {}
        };
    }
}

#endif  /* __CONFIG_INPUT_FILE_H__ */
