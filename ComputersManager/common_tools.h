#pragma once
#include <iostream>
#include "sql_service.h"
#include <qstandarditemmodel.h>
#include <string>
#include "ctime"
#include "iomanip"
#include "sstream"
#include "cmath"

#define LOGIN_DEBUG 1
#define REGISTER_DEBUG 1
#define MODEIFY_DEBUG 1
#define CREATE_ROOM_DEBUG 1
#define MODIFY_MACHINE_DEBUG 1


#define BDEBUG(s) std::cout << __FILE__ <<"   "<< s << std::endl;		// 一个宏函数，调试用的
std::string TransPermissionCodeToString(unsigned int permission);		// 把权限值转换为对应的字符串
void TableClear(QStandardItemModel* model);								// QTableview的清空函数
time_t MysqlDateTimeToTimeT(const std::string& datetime);				
double CalculateHourDifference(const std::string& datetime1, const std::string& datetime2);

