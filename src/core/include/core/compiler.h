#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <string>

namespace execHelper {
    namespace core {
        class Compiler {
            public:
                Compiler(const std::string& name) :
                    m_name(name) 
                {
                    ;
                }

                const std::string& getName() const noexcept {
                    return m_name;
                }

                bool operator==(const Compiler& other) const noexcept {
                    return m_name == other.m_name;
                }

                bool operator!=(const Compiler& other) const noexcept {
                    return !(*this == other);
                }

            private:
                std::string m_name;
        };
    }
}

#endif /* __COMPILER_H__ */
