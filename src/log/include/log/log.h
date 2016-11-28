#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define LOG(x) (std::cout << __FILE__ << __LINE__ << x << std::endl)
#define user_feedback(x) (LOG(x))

#endif  /* __LOG_H__ */
