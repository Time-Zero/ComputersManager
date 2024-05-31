#include "sql_service.h"

SqlService::SqlService(std::string ip, std::string user, std::string password) {
	try
	{
		BDEBUG("SqlService Structure")

		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
		p_conn_ = driver->connect(ip, user, password);
		p_stat_ = p_conn_->createStatement();
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}
}

SqlService::~SqlService()
{
	BDEBUG("SqlService DisStructure");
	delete p_stat_;
	delete p_conn_;
}

SqlService& SqlService::GetInstance()
{
	static SqlService ins;
	return ins;
}

UserInfo SqlService::GetUserInfo(std::string& userid)
{
	UserInfo user_info;
	
	return user_info;
}

std::string SqlService::GetUserPassword(std::string& userid)
{
	sql::ResultSet* res = nullptr;
	std::string password = "";
	std::string sql = "select " UI_PASSWORD
		" from " USER_INFO_TABLE
		" where " UI_ID " = " "'" + userid + "'";
	
	BDEBUG(sql);
	
	try
	{
		res = p_stat_->executeQuery(sql);
		if (res->rowsCount() != 0) {
			res->next();
			password = res->getString(UI_PASSWORD);
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}
	delete res;

	return password;
}

unsigned int SqlService::Register(UserInfo& user_info)
{
	int ret = 0;
	std::string sql = "insert into " USER_INFO_TABLE "("
		UI_ID ","
		UI_NAME ","
		UI_PASSWORD ")"
		" values ("
		"'" + user_info.id + "',"
		"'" + user_info.name + "',"
		"'" + user_info.password + "')";

	BDEBUG(sql);

	try
	{
		p_stat_->execute(sql);
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		ret = 1;
	}

	return ret;
}
