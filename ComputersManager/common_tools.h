#pragma once
#include <iostream>
#include "sql_service.h"
#include <qstandarditemmodel.h>

#define LOGIN_DEBUG 1
#define REGISTER_DEBUG 1
#define MODEIFY_DEBUG 1
#define CREATE_ROOM_DEBUG 1
#define MODIFY_MACHINE_DEBUG 1


#define BDEBUG(s) std::cout << __FILE__ <<"   "<< s << std::endl;
std::string TransPermissionCodeToString(unsigned int permission);
void TableClear(QStandardItemModel* model);
