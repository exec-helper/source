#ifndef __MODE_H__
#define __MODE_H__

#include <string>

namespace execHelper {
    namespace core {
        class Mode {
            public:
                virtual const std::string& getMode() const noexcept {
                    return m_name;
                }

                virtual bool operator==(const Mode& other) const noexcept {
                    return m_name == other.m_name;
                }

                virtual bool operator!=(const Mode& other) const noexcept {
                    return !(*this == other);
                }

            protected:
                Mode(const std::string& modeName) :
                    m_name(modeName)
                {
                    ;
                }

            private:
                Mode() = delete;

                std::string m_name;
        };

        class Release : public Mode {
            public:
                Release() :
                    Mode("release")
                {
                    ;
                }
        };

        class Debug : public Mode {
            public:
                Debug() :
                    Mode("debug")
                {
                    ;
                }
        };
    }
}

#endif /* __MODE_H__ */
