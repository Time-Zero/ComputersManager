#pragma once
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/mysql_driver.h>
#include <string>
#include "common_tools.h"

#define SQL_IP "tcp://172.30.131.79:3306/CMDB"
#define SQL_USER "admin"
#define SQL_PASSWORD "334859"


class SqlService
{
public:
	SqlService(const SqlService&) = delete;
	SqlService& operator=(const SqlService&) = delete;

	static SqlService& GetInstance();

private:
	SqlService(std::string ip = SQL_IP, std::string user = SQL_USER, std::string password = SQL_PASSWORD);
	~SqlService();
private:
	sql::Connection* p_conn_;
	sql::Statement* p_stat_;

};

