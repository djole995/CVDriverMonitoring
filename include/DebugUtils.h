#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

#include <iostream>


#ifdef DEBUG_VERSION
#define DEBUG_LOG(str) (std::cout << str << std::endl)
#else
#define DEBUG_LOG(str)
#endif



#endif
