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

void TableClear(QStandardItemModel* model)
{
	int rowCount = model->rowCount();
	for (int i = 0; i < rowCount; i++) {
		model->removeRow(0);
	}
}


/// @brief 时间字符串转时间
/// @param datetime 时间字符串
/// @return 时间
time_t MysqlDateTimeToTimeT(const std::string& datetime)
{
	std::tm tm = {};
	std::istringstream ss(datetime);
	ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
	if (ss.fail()) {
		throw std::runtime_error("Failed to parse date time string");
	}

	return mktime(&tm);
}

/// @brief 计算两个时间之间的小时差
/// @param datetime1 
/// @param datetime2 
/// @return 小时差
double CalculateHourDifference(const std::string& datetime1, const std::string& datetime2)
{
	time_t time1 = MysqlDateTimeToTimeT(datetime1);
	time_t time2 = MysqlDateTimeToTimeT(datetime2);

	double seconds_diff = std::difftime(time1, time2);
	double hours_diff = seconds_diff / 3600.0;

	return hours_diff;
}

