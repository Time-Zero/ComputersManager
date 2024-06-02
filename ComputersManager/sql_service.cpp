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


void SqlService::GetUserInfo(UserInfo& user_info)
{
	sql::ResultSet* res = nullptr;
	std::string sql = "select " 
		UI_NAME ","
		UI_PASSWORD ","
		UP_PERMISSION ","
		UP_ORDER
		" from "
		USER_INFO_TABLE ","
		USER_PERMISSION_TABLE
		" where " UI_ID " = " UP_ID " and "
		UI_ID "=" + user_info.id;

	BDEBUG(sql);
	try
	{
		if (p_stat_) {
			res = p_stat_->executeQuery(sql);
		}
		else {
			std::this_thread::sleep_for(std::chrono::microseconds(SQL_DELAYTIME));
		}

		if (res) {
			res->next();
			user_info.name = res->getString(UI_NAME);
			user_info.password = res->getString(UI_PASSWORD);
			user_info.permission = res->getInt(UP_PERMISSION);
			user_info.order = res->getInt(UP_ORDER);
			delete res;
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}
	
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
		if (p_stat_) {
			res = p_stat_->executeQuery(sql);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(SQL_DELAYTIME));
		}
		
		if (res && res->rowsCount() != 0) {
			res->next();
			password = res->getString(UI_PASSWORD);
			delete res;
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}

	return password;
}

std::string SqlService::GetUserName(std::string& userid, unsigned int userpermission)
{
	std::string user_name = "";
	sql::ResultSet* res = nullptr;
	std::string sql = "select "
		UI_NAME
		" from "
		USER_INFO_TABLE "," 
		USER_PERMISSION_TABLE
		" where "
		UI_ID " = " UP_ID " and "
		UI_ID " = '" + userid + "' and " 
		UP_PERMISSION " < " + std::to_string(userpermission);

	BDEBUG(sql);
	try
	{
		if (p_stat_) {
			res = p_stat_->executeQuery(sql);
			if (res->rowsCount()) {
				res->next();
				user_name = res->getString(UI_NAME);
			}
		}

		if (res)
			delete res;
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		std::this_thread::sleep_for(std::chrono::microseconds(SQL_DELAYTIME));
	}

	return user_name;
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
		if (p_stat_) {
			p_stat_->execute(sql);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(SQL_DELAYTIME));
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		ret = 1;
	}

	return ret;
}

unsigned int SqlService::ModifyInfo(UserInfo& user_info)
{
	unsigned int ret = 0;
	std::string sql = "update "
		USER_INFO_TABLE
		" set "
		UI_NAME "='" + user_info.name + "',"
		UI_PASSWORD "='" + user_info.password + "'"
		" where " UI_ID "=" + "'" + user_info.id + "'";
	BDEBUG(sql);

	try {
		if (p_stat_) {
			p_stat_->executeUpdate(sql);
		}
		else {
			std::this_thread::sleep_for(std::chrono::microseconds(SQL_DELAYTIME));
		}
	}
	catch (const sql::SQLException& e) {
		BDEBUG(e.what());
		ret = 1;
	}

	return ret;
}

std::queue<UserInfo> SqlService::GetUserList(UserInfo& user_info)
{
	std::queue<UserInfo> ret;
	sql::ResultSet* res = nullptr;
	std::string sql = "select "
		UI_ID ","
		UI_NAME
		" from " USER_INFO_TABLE ","
		USER_PERMISSION_TABLE
		" where " UI_ID "=" UP_ID
		" and " UP_PERMISSION " < " + std::to_string(user_info.permission);
	BDEBUG(sql);
	
	try
	{
		if (p_stat_) {
			res = p_stat_->executeQuery(sql);
			while(res->next()){
				UserInfo temp;
				temp.id = res->getString(UI_ID);
				temp.name = res->getString(UI_NAME);
				/*temp.password = res->getString(UI_PASSWORD);
				temp.permission = res->getInt(UP_PERMISSION);
				temp.order = res->getInt(UP_ORDER);*/

				ret.emplace(std::move(temp));
			}
			
			if(res) 
				delete res;
		}
		else {
			return ret;
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}

	return ret;
}

unsigned int SqlService::ChangInfo(UserInfo& user_info)
{
	unsigned int ret = 0;
	std::string sql_for_user_info = "update " USER_INFO_TABLE
		" set " UI_NAME "='" + user_info.name + "',"
		UI_PASSWORD "='" + user_info.password + "'"
		" where " UI_ID "='" + user_info.id + +"'";

	std::string sql_for_user_permission = "update " USER_PERMISSION_TABLE 
		" set " UP_PERMISSION "=" + std::to_string(user_info.permission) + ","
			UP_ORDER "=" + std::to_string(user_info.order) + 
		" where " UP_ID "='" + user_info.id + +"'";

	BDEBUG(sql_for_user_info);
	BDEBUG(sql_for_user_permission);

	try
	{
		if (p_conn_) {
			p_conn_->setAutoCommit(false);
		}
		else {
			return ret;
		}

		if (p_stat_) {
			p_stat_->executeUpdate(sql_for_user_info);
			p_stat_->executeUpdate(sql_for_user_permission);
		}
		else {
			return ret;
		}

		p_conn_->commit();
		p_conn_->setAutoCommit(true);
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		p_conn_->rollback();
		ret = 1;
	}

	return ret;
}
