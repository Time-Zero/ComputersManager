#include "configure_get.h"

ConfigureGet::ConfigureGet()
{
	std::ifstream in_file("config.json", std::ios::in);
	in_file >> j_;
}

ConfigureGet::~ConfigureGet()
{
}

std::vector<std::string> ConfigureGet::GetSqlConfig()
{
	std::vector<std::string> ret;
	ret.emplace_back(j_["server"]["ip"].get<std::string>());
	ret.emplace_back(j_["database"]["port"].get<std::string>());
	ret.emplace_back(j_["database"]["user"].get<std::string>());
	ret.emplace_back(j_["database"]["password"].get<std::string>());
	return ret;
}

void ConfigureGet::GetServerConfig(ServerInfo& server_info)
{
	server_info.ip = j_["server"]["ip"].get<std::string>();
	server_info.port = j_["server"]["port"].get<int>();
	server_info.user = j_["server"]["user"].get<std::string>();
	server_info.password = j_["server"]["password"].get<std::string>();
}
