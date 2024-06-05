#pragma once
#include "common_tools.h"
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/mysql_driver.h>
#include <mysqlx/xdevapi.h>
#include <string>
#include <thread>
#include <queue>
#include <memory>

#define SQL_IP "tcp://172.30.131.79:3306/CMDB"
#define SQL_USER "admin"
#define SQL_PASSWORD "334859"

#define USER_INFO_TABLE "user_info"
#define	USER_PERMISSION_TABLE "user_permission"
#define MACHINE_ROOM_TABLE "machine_room"

#define PROCEDURE_CREATE_ROOM_MACHINE "CreateTableAndInsertDefaultData"

#define UI_ID "ui_id"
#define UI_NAME "ui_name"
#define UI_PASSWORD "ui_password"

#define UP_ID "up_id"
#define UP_PERMISSION "up_permission"
#define UP_ORDER "up_order"

#define MR_NAME "mr_name"
#define MR_STATUS "mr_status"
#define MR_MANAGER "mr_manager"

#define MH_ID "mh_id"
#define MH_STATUS "mh_status"
#define MH_CPU "mh_cpu"
#define MH_RAM "mh_ram"
#define MH_ROM "mh_rom"
#define MH_GPU "mh_gpu"
#define MH_UID "mh_uid"
#define MH_SDATE "mh_sdate"

#define SQL_TIMEOUT 2000
#define SQL_DELAYTIME 2200

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

typedef struct MachineInfo {
	std::string room_name = "";
	std::string cpu = "";
	std::string ram = "";
	std::string rom = "";
	std::string gpu = "";
	std::string mananger_id = "";
	unsigned int machine_num = 0;
} MachineInfo;

class SqlService
{
public:
	SqlService(const SqlService&) = delete;
	SqlService& operator=(const SqlService&) = delete;

	static SqlService& GetInstance();

	void GetUserInfo(UserInfo& user_info);
	void GetManagerInfo(UserInfo& user_info);
	std::string GetUserPassword(std::string& userid);
	std::string GetUserName(std::string& userid, unsigned int userpermission);
	unsigned int Register(UserInfo& user_info);
	unsigned int ModifyInfo(UserInfo& user_info);
	std::queue<UserInfo> GetUserList(UserInfo& user_info);
	std::vector<std::vector<std::string>> GetRoomList();
	unsigned int ChangInfo(UserInfo& user_info); 
	unsigned int CreateRoom(MachineInfo& machine_info);
	std::vector<std::string> GetRoomInfo(std::string& room_name);
	unsigned int ModifyRoomInfo(std::string& room_name, std::pair<int, std::string> new_info);
	unsigned int DeleteMachineRoom(std::string& room_name);
	unsigned int GetPeopleOnUseMachine(std::string& room_name);


private:
	SqlService(std::string ip = SQL_IP, std::string user = SQL_USER, std::string password = SQL_PASSWORD);
	~SqlService();
private:
	/*sql::Connection* p_conn_;
	sql::Statement* p_stat_;*/
	std::shared_ptr<sql::Connection> p_conn_;				// sql 连接
	std::shared_ptr<sql::Statement> p_stat_;				// autocommit
};

