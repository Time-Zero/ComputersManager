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

#define USER_INFO_TABLE "user_info"
#define	USER_PERMISSION_TABLE "user_permission"

#define UI_ID "ui_id"
#define UI_NAME "ui_name"
#define UI_PASSWORD "ui_password"

#define UP_ID "up_id"
#define UP_PERMISSION "up_permission"
#define UP_ORDER "up_order"

enum PermissionEnum
{
	USER = 1,
	ADMIN = 2,
	SADMIN = 3
};

typedef struct UserInfo {
	std::string id = "";
	std::string name = "";
	std::string password = "";
	unsigned int permission = 0;
	unsigned int order = 0;
} UserInfo;

class SqlService
{
public:
	SqlService(const SqlService&) = delete;
	SqlService& operator=(const SqlService&) = delete;

	static SqlService& GetInstance();

	UserInfo GetUserInfo(std::string& userid);
	std::string GetUserPassword(std::string& userid);
	unsigned int Register(UserInfo& user_info);

private:
	SqlService(std::string ip = SQL_IP, std::string user = SQL_USER, std::string password = SQL_PASSWORD);
	~SqlService();
private:
	sql::Connection* p_conn_;
	sql::Statement* p_stat_;

};

