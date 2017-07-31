#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#include <ostream>

namespace execHelper {
    namespace color {
        enum ColorCode {
            FG_RED      = 31,
            FG_GREEN    = 32,
            FG_BLUE     = 34,
            FG_DEFAULT  = 39,
            BG_RED      = 41,
            BG_GREEN    = 42,
            BG_BLUE     = 44,
            BG_DEFAULT  = 49
        };

        /**
         * \brief Modifies the command line output format
         */
        class Modifier {
            ColorCode code;
        public:
            Modifier(ColorCode cCode) : code(cCode) {}
            friend std::ostream&
            operator<<(std::ostream& os, const Modifier& mod) {
                return os << "\033[" << mod.code << "m";
            }

        };

        static Modifier def(FG_DEFAULT);
        static Modifier blue(FG_BLUE);
        static Modifier red(FG_RED);
    }
}

#define LOG(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t" << x << std::endl)
#define user_feedback(x) (std::cout << x << std::endl)
#define user_feedback_info(x) (std::cout << execHelper::color::blue << x << execHelper::color::def << std::endl);
#define user_feedback_error(x) (std::cout << execHelper::color::red << x << execHelper::color::def << std::endl);

#endif  /* __LOG_H__ */
