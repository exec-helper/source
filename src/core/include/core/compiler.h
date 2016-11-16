#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <string>

namespace execHelper {
    namespace core {
        class Compiler {
            public:
                virtual const std::string& getName() const noexcept {
                    return m_name;
                }

                virtual bool operator==(const Compiler& other) const noexcept {
                    return m_name == other.m_name;
                }

                virtual bool operator!=(const Compiler& other) const noexcept {
                    return !(*this == other);
                }

            protected:
                Compiler(const std::string& name) :
                    m_name(name) 
                {
                    ;
                }

            private:
                std::string m_name;
        };

        class Gcc : public Compiler {
            public:
                Gcc() :
                    Compiler("gcc")
                {
                    ;
                }
        };

        class Clang : public Compiler {
            public:
                Clang() :
                    Compiler("clang")
                {
                    ;
                }
        };
    }
}

#endif /* __COMPILER_H__ */
