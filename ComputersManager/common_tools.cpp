#include "common_tools.h"

std::string TransPermissionCodeToString(unsigned int permission)
{
	std::string ret = "";
	switch (permission)
	{
	case USER:
		ret = "用户";
		break;
	case ADMIN:
		ret = "管理员";
		break;
	case SADMIN:
		ret = "超级管理员";
		break;
	default:
		ret = "未知";
		break;
	}

	return ret;
}
