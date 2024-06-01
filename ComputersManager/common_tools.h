#pragma once
#include <iostream>
#include "sql_service.h"

#define LOGIN_DEBUG 1
#define REGISTER_DEBUG 1
#define MODEIFY_DEBUG 1
 
#define BDEBUG(s) std::cout << __FILE__ <<"   "<< s << std::endl;
std::string TransPermissionCodeToString(unsigned int permission);
