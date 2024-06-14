#include "configure_get.h"

/// @brief 构造函数
ConfigureGet::ConfigureGet()
{
	std::ifstream in_file("config.json", std::ios::in);
	in_file >> j_;
}

/// @brief 析构函数
ConfigureGet::~ConfigureGet()
{
}

/// @brief 读取数据库配置文件
/// @return 返回数据库ip、端口、用户名、密码
std::vector<std::string> ConfigureGet::GetSqlConfig()
{
	std::vector<std::string> ret;
	ret.emplace_back(j_["server"]["ip"].get<std::string>());
	ret.emplace_back(j_["database"]["port"].get<std::string>());
	ret.emplace_back(j_["database"]["user"].get<std::string>());
	ret.emplace_back(j_["database"]["password"].get<std::string>());
	return ret;
}

/// @brief 返回服务器信息
/// @param server_info 包括服务器ip、端口、用户名、密码
void ConfigureGet::GetServerConfig(ServerInfo& server_info)
{
	server_info.ip = j_["server"]["ip"].get<std::string>();
	server_info.port = j_["server"]["port"].get<int>();
	server_info.user = j_["server"]["user"].get<std::string>();
	server_info.password = j_["server"]["password"].get<std::string>();
}
