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