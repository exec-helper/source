#ifndef __MODE_H__
#define __MODE_H__

#include <string>

namespace execHelper {
    namespace core {
        class Mode {
            public:
                Mode(const std::string& modeName) :
                    m_name(modeName)
                {
                    ;
                }


                virtual const std::string& getMode() const noexcept {
                    return m_name;
                }

                virtual bool operator==(const Mode& other) const noexcept {
                    return m_name == other.m_name;
                }

                virtual bool operator!=(const Mode& other) const noexcept {
                    return !(*this == other);
                }

            private:
                std::string m_name;
        };
    }
}

#endif /* __MODE_H__ */
