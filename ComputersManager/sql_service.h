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
#include <vector>

#define SQL_IP "tcp://172.30.131.79:3306/CMDB"
#define SQL_USER "admin"
#define SQL_PASSWORD "334859"

#define USER_INFO_TABLE "user_info"
#define	USER_PERMISSION_TABLE "user_permission"
#define MACHINE_ROOM_TABLE "machine_room"

#define PROCEDURE_CREATE_ROOM_MACHINE "CreateTableAndInsertDefaultData"
#define PROCEDURE_CHECK_SOMEONE_IS_RENT "CheckSomeOneIsRent"

#define UI_ID "ui_id"
#define UI_NAME "ui_name"
#define UI_PASSWORD "ui_password"

#define UP_ID "up_id"
#define UP_PERMISSION "up_permission"
#define UP_ORDER "up_order"

#define MR_NAME "mr_name"
#define MR_STATUS "mr_status"
#define MR_MANAGER "mr_manager"
#define MR_FEES "mr_fees"

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
	double machine_fees = 0;
	unsigned int machine_num = 0;
} MachineInfo;

typedef struct Machine {
	int id = 0;
	int status = 0;
	std::string cpu = "";
	std::string ram = "";
	std::string rom = "";
	std::string gpu = "";
	std::string uid = "";
	std::string mh_sdata = "";
	std::string uname = "";
} Machine;

class SqlService
{
public:
	SqlService(const SqlService&) = delete;
	SqlService& operator=(const SqlService&) = delete;

	static SqlService& GetInstance();

	void GetUserInfo(UserInfo& user_info);							//获取用户信息，登录后主页显示用的
	void GetManagerInfo(UserInfo& user_info);						//主要是为了查权限
	std::string GetUserPassword(std::string& userid);				//获取用户密码
	std::string GetUserName(std::string& userid, unsigned int userpermission);			// 获取用户名，设置管理员用的，只能返回权限比自己小的人
	std::string GetUserName(std::string& userid);
	unsigned int Register(UserInfo& user_info);					// 注册用的
	unsigned int ModifyInfo(UserInfo& user_info);				// 自己主动修改信息
	std::queue<UserInfo> GetUserList(UserInfo& user_info);			// 获取用户列表
	std::vector<std::vector<std::string>> GetRoomList();			//机房列表
	unsigned int ChangInfo(UserInfo& user_info);					// 管理员修改用户信息
	unsigned int CreateRoom(MachineInfo& machine_info);				// 创建机房
	std::vector<std::string> GetRoomInfo(std::string& room_name);				//获取机房信息
	int ModifyRoomInfo(std::string& room_name, std::pair<int, std::string> new_info, double fees);		// 修改机房信息
	unsigned int DeleteMachineRoom(std::string& room_name);				// 删除机房
	int GetPeopleOnUseMachine(std::string& room_name);			// 获取正在使用指定机房的人数
	std::queue<Machine> GetMachines(std::string& room_name);			// 获取电脑列表
	unsigned int DeleteMachine(std::string& machine_id, std::string& machine_room);			// 删除电脑
	unsigned int AddMachine(std::string& room_name, Machine& machine);		// 添加电脑
	int GetMachineStatus(std::string& room_name, std::string& machine_id);
	int ModifyMachineInfo(Machine& machine, std::string& room_name);
	std::vector<std::string> GetMachineUser(std::string& room_name, std::string& machine_id);
	int CheckSomeOneIsRent(std::string& user_id);				// 检查这个人是不是在别的机房有记录


private:
	SqlService(std::string ip = SQL_IP, std::string user = SQL_USER, std::string password = SQL_PASSWORD);
	~SqlService();
private:
	/*sql::Connection* p_conn_;
	sql::Statement* p_stat_;*/
	std::shared_ptr<sql::Connection> p_conn_;				// sql 连接
	std::shared_ptr<sql::Statement> p_stat_;				// autocommit
};

