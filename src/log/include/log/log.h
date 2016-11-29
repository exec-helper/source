#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define LOG(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t" << x << std::endl)
#define user_feedback(x) (std::cout << x << std::endl)

#endif  /* __LOG_H__ */
