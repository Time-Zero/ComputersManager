#pragma once
#include "nlohmann/json.hpp"
#include "fstream"
#include "common_tools.h"

typedef struct ServerInfo {
	std::string ip;
	int port = 22;
	std::string user;
	std::string password;
} ServerInfo;

class ConfigureGet
{
public:
	ConfigureGet();
	~ConfigureGet();
	std::vector<std::string> GetSqlConfig();
	void GetServerConfig(ServerInfo& server_info);


private:
	nlohmann::json j_;
};

