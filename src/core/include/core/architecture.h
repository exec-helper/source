#ifndef __ARCHITECTURE_H__
#define __ARCHITECTURE_H__

#include <string>

namespace execHelper {
    namespace core {
        class Architecture {
            public:
                Architecture(const std::string& architectureName) :
                    m_architecture(architectureName)
                {
                    ;
                }

                virtual const std::string& getArchitecture() const noexcept {
                    return m_architecture;
                }

                virtual bool operator==(const Architecture& other) const noexcept {
                    return m_architecture == other.m_architecture;
                }

                virtual bool operator!=(const Architecture& other) const noexcept {
                    return !(*this == other);
                }

            private:
                std::string m_architecture;
        };
    }
}

#endif /* __ARCHITECTURE_H__ */
