#include "sql_service.h"

/// @brief 构造函数
/// @param ip 数据库IP
/// @param user 登录用户名
/// @param password 密码
SqlService::SqlService(std::string ip, std::string user, std::string password) {
	try
	{
		BDEBUG("SqlService Structure")

		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
		p_conn_.reset(driver->connect(ip, user, password));
		p_stat_.reset(p_conn_->createStatement());
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}
}

/// @brief 析构函数
SqlService::~SqlService()
{
	BDEBUG("SqlService DisStructure");
}


/// @brief 单例获取
/// @return 返回单例对象
SqlService& SqlService::GetInstance()
{
	static SqlService ins;
	return ins;
}


/// @brief 获取用户信息，传入id，通过引用返回
/// @param user_info 需要设置user_info的id
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


void SqlService::GetManagerInfo(UserInfo& user_info)
{
	sql::ResultSet* res = nullptr;
	std::string sql = "select "
		UI_NAME ","
		UP_PERMISSION
		" from " USER_INFO_TABLE ","
		USER_PERMISSION_TABLE
		" where " UI_ID "=" UP_ID " and "
		UI_ID "='" + user_info.id + "'";
	BDEBUG(sql);

	try
	{
		if (p_stat_) {
			res = p_stat_->executeQuery(sql);
		}

		if (res) {
			if (res->rowsCount()) {
				res->next();
				user_info.name = res->getString(UI_NAME);
				user_info.permission = res->getInt(UP_PERMISSION);
			}
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
		UI_NAME ","
		UP_PERMISSION ","
		UP_ORDER
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
				temp.permission = res->getInt(UP_PERMISSION);
				temp.order = res->getInt(UP_ORDER);

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

std::vector<std::vector<std::string>> SqlService::GetRoomList()
{
	std::vector<std::vector<std::string>> ret;
	sql::ResultSet* res = nullptr;
	std::string sql = "select " MR_NAME ","
		MR_STATUS ","
		MR_MANAGER ","
		MR_FEES ","
		UI_NAME
		" from "
		MACHINE_ROOM_TABLE ","
		USER_INFO_TABLE
		" where " UI_ID "=" MR_MANAGER;
	BDEBUG(sql);

	try
	{
		res = p_stat_->executeQuery(sql);
		while (res->next()) {
			std::vector<std::string> temp;
			temp.emplace_back(res->getString(MR_NAME));
			temp.emplace_back(res->getString(MR_STATUS));
			temp.emplace_back(res->getString(MR_FEES));
			temp.emplace_back(res->getString(MR_MANAGER));
			temp.emplace_back(res->getString(UI_NAME));
			ret.emplace_back(temp);
		}

		if (res)
			delete res;
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

	p_conn_->setAutoCommit(false);
	try
	{
		p_stat_->executeUpdate(sql_for_user_info);
		p_stat_->executeUpdate(sql_for_user_permission);

		p_conn_->commit();
	}
	catch (const sql::SQLException& e)
	{
		try
		{
			p_conn_->rollback();
		}
		catch (const sql::SQLException& e)
		{
			BDEBUG(e.what());
		}
		ret = 1;
		BDEBUG(e.what());
	}

	p_conn_->setAutoCommit(true);
	return ret;
}

unsigned int SqlService::CreateRoom(MachineInfo& machine_info)
{
	unsigned int ret = 0;
	std::string sql_for_room = "insert into " MACHINE_ROOM_TABLE
		"(" MR_NAME ","
		MR_MANAGER ","
		MR_FEES
		")"
		" values ("
		"'" + machine_info.room_name + "',"
		"'" + machine_info.mananger_id + "'," 
		"'" + std::to_string(machine_info.machine_fees) + "')";

	std::string sql_for_machine = "call " PROCEDURE_CREATE_ROOM_MACHINE "('" + machine_info.room_name + "',"
		+ std::to_string(machine_info.machine_num) + ",'" + machine_info.cpu + "','"
		+ machine_info.ram + "','" + machine_info.rom + "','" + machine_info.gpu + "')";

	p_conn_->setAutoCommit(false);
	try
	{
		p_stat_->execute(sql_for_machine);
		p_stat_->execute(sql_for_room);
		p_conn_->commit();

	}
	catch (const sql::SQLException& e)
	{
		try
		{
			p_conn_->rollback();
		}
		catch (const std::exception& e)
		{
			BDEBUG(e.what());
		}
		ret = 1;
		BDEBUG(e.what());
	}
	p_conn_->setAutoCommit(true);

	return ret;
}

std::vector<std::string> SqlService::GetRoomInfo(std::string& room_name)
{
	std::vector<std::string> ret;
	sql::ResultSet* res = nullptr;
	std::string sql = "select " MR_STATUS ","
		MR_MANAGER
		" from " MACHINE_ROOM_TABLE
		" where " MR_NAME "='" + room_name + "'";
	
	BDEBUG(sql);
	try
	{
		res = p_stat_->executeQuery(sql);
		if (res->next()) {
			ret.emplace_back(res->getString(MR_STATUS));
			ret.emplace_back(res->getString(MR_MANAGER));
		}

		if (res) {
			delete res;
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}

	return ret;
}

/// @brief 修改机房信息
/// @param room_name 机房名
/// @param new_info 要修改的数据
/// @return 0：修改成功 -1：未知错误 1：由于有人正在使用，所以无法停用 
int SqlService::ModifyRoomInfo(std::string& room_name, std::pair<int, std::string> new_info, double fees)
{
	unsigned ret = 0;
	std::vector<std::string> sql_vec;

	if (new_info.first != -1) {	// 需要修改状态
		int people_on_use = this->GetPeopleOnUseMachine(room_name);
		if (people_on_use == -1)		// 未知错误
			return -1;
		else if (people_on_use > 0)			//   有人还在使用
			return 1;

		std::string sql = "update " MACHINE_ROOM_TABLE
			" set " MR_STATUS "=" + std::to_string(new_info.first) +
			" where " MR_NAME "='" + room_name + "'";
		sql_vec.emplace_back(sql);
	}

	if (!new_info.second.empty()) {		// 需要修改管理员
		std::string sql = "update " MACHINE_ROOM_TABLE
					" set " MR_MANAGER "='" + new_info.second + "'"
					" where " MR_NAME "='" + room_name + "'";
		sql_vec.emplace_back(sql);
	}

	if (fees != double(0)) {
		std::string sql = "update " MACHINE_ROOM_TABLE
			" set " MR_FEES "=" + std::to_string(fees) +
			" where " MR_NAME "='" + room_name + "'";
		sql_vec.emplace_back(sql);
	}

	try
	{
		for (auto sql : sql_vec) {
			p_stat_->executeUpdate(sql);
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		ret = -1;
	}


	return ret;
}

/// @brief 删除机房
/// @param room_name 机房名
/// @return 0:正常删除 1：机房还有人在用 2：未知错误
unsigned int SqlService::DeleteMachineRoom(std::string& room_name)
{
	unsigned int ret = 0;
	unsigned int people_on_use = this->GetPeopleOnUseMachine(room_name);
	if (people_on_use == -1) {			// 为-1，则出现查找错误
		ret = 2;
	}
	else if (people_on_use > 0) {		// >0，说明还有人再用
		ret = 1;
	}

	if (ret != 0)			// 如果还有人使用，就返回，不执行后续删除操作
		return ret;

	std::string sql_for_delete_table = "drop table " + room_name;
	std::string sql_for_delete_table_record = "delete from " MACHINE_ROOM_TABLE " where " MR_NAME "='" + room_name + "'";
	
	try
	{
		p_stat_->execute(sql_for_delete_table);
		p_stat_->execute(sql_for_delete_table_record);
	}
	catch (const sql::SQLException& e)
	{
		ret = 2;
		BDEBUG(e.what());
	}

	return ret;
}

/// @brief 统计机房正在使用人数
/// @param room_name 机房名
/// @return 人数>=0 ，如果人数==-1，则查找错误
int SqlService::GetPeopleOnUseMachine(std::string& room_name)
{
	int ret = -1;
	sql::ResultSet* res = nullptr;
	std::string sql_for_confirm_no_people_use = "select count(" MH_UID ") ucount"
		" from " + room_name +
		" where " MH_UID " is not null";

	BDEBUG(sql_for_confirm_no_people_use);
	// 计算有多少人正在使用
	try
	{
		res = p_stat_->executeQuery(sql_for_confirm_no_people_use);
		while (res->next()) {
			ret = res->getInt("ucount");
		}
		delete res;
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}

	return ret;
}

std::queue<Machine> SqlService::GetMachines(std::string& room_name)
{
	std::queue<Machine> ret;
	sql::ResultSet* res = nullptr;
	std::string sql = "select "
		MH_ID ","
		MH_STATUS ","
		MH_CPU ","
		MH_RAM ","
		MH_ROM ","
		MH_GPU ","
		MH_UID ","
		UI_NAME " from " + room_name + " left join " USER_INFO_TABLE " on " UI_ID "=" MH_ID;
	BDEBUG(sql);
	
	try
	{
		res = p_stat_->executeQuery(sql);
		while (res->next()) {
			Machine machine;
			machine.id = res->getInt(MH_ID);
			machine.status = res->getInt(MH_STATUS);
			machine.cpu = res->getString(MH_CPU);
			machine.ram = res->getString(MH_RAM);
			machine.rom = res->getString(MH_ROM);
			machine.gpu = res->getString(MH_GPU);
			machine.uid = res->getString(MH_UID);
			machine.uname = res->getString(UI_NAME);

			ret.push(machine);
		}

		delete res;
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
	}
						
	return ret;
}

unsigned int SqlService::DeleteMachine(std::string& machine_id, std::string& machine_room)
{
	unsigned int ret = 0;
	std::string sql = "delete from " + machine_room + " where " MH_ID " = " + machine_id;
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

unsigned int SqlService::AddMachine(std::string& room_name, Machine& machine)
{
	unsigned int ret = 0;
	std::string sql;
	if (machine.gpu.empty()) {
		sql = "insert into " + room_name + "(" MH_CPU "," MH_RAM "," MH_ROM ") values('" + machine.cpu + "','" +
			machine.ram + "','" + machine.rom + "')";
	}
	else {
		sql = "insert into " + room_name + "(" MH_CPU "," MH_RAM "," MH_ROM "," MH_GPU ") values('" + machine.cpu + "','" +
			machine.ram + "','" + machine.rom + "','" + machine.gpu + "')";
	}

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

/// @brief 返回电脑状态
/// @param room_name 机房名
/// @param machine_id 电脑编号
/// @return	0：停用，1：空闲，2：使用，-1：未知错误
int SqlService::GetMachineStatus(std::string& room_name, std::string& machine_id)
{
	int ret = 0;
	sql::ResultSet* res = nullptr;
	std::string sql = "select " MH_STATUS " from " + room_name + " where " MH_ID "=" + machine_id;
	BDEBUG(sql);

	try
	{
		res = p_stat_->executeQuery(sql);
		while (res->next()) {
			ret = res->getInt(MH_STATUS);
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		ret = -1;			
	}

	return ret;
}

int SqlService::ModifyMachineInfo(Machine& machine, std::string& room_name)
{
	int ret = 0;
	std::vector<std::string> sql_vec;
	
	if (!machine.cpu.empty()) {
		std::string sql = "update " + room_name + 
			" set " MH_CPU "='" + machine.cpu + 
			"' where " MH_ID "=" + std::to_string(machine.id);
		BDEBUG(sql);
		sql_vec.emplace_back(sql);
	}

	if (!machine.ram.empty()) {
		std::string sql = "update " + room_name +
			" set " MH_RAM "='" + machine.ram +
			"' where " MH_ID "=" + std::to_string(machine.id);
		BDEBUG(sql);
		sql_vec.emplace_back(sql);
	}

	if (!machine.rom.empty()) {
		std::string sql = "update " + room_name +
			" set " MH_ROM "='" + machine.rom +
			"' where " MH_ID "=" + std::to_string(machine.id);
		BDEBUG(sql);
		sql_vec.emplace_back(sql);
	}

	if (!machine.gpu.empty()) {
		std::string sql = "update " + room_name +
			" set " MH_GPU "='" + machine.gpu +
			"' where " MH_ID "=" + std::to_string(machine.id);
		BDEBUG(sql);
		sql_vec.emplace_back(sql);
	}

	if (machine.status != -1) {
		std::string sql = "update " + room_name +
			" set " MH_STATUS "=" + std::to_string(machine.status) +
			" where " MH_ID "=" + std::to_string(machine.id);
		BDEBUG(sql);
		sql_vec.emplace_back(sql);
	}

	try
	{
		for (auto sql : sql_vec) {
			p_stat_->executeUpdate(sql);
		}
	}
	catch (const sql::SQLException& e)
	{
		BDEBUG(e.what());
		ret = -1;
	}

	return ret;
}
