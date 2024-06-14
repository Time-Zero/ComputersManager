#include "common_tools.h"

/// @brief 权限值转权限字符串
/// @param permission 权限值
/// @return 对应的权限的字符串
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

/// @brief 清空Qtableview的内容
/// @param model qtableview的model
void TableClear(QStandardItemModel* model)
{
	int rowCount = model->rowCount();
	for (int i = 0; i < rowCount; i++) {		// 循环清空table的顶层
		model->removeRow(0);
	}
}


/// @brief 时间字符串转时间
/// @param datetime 时间字符串
/// @return 时间
time_t MysqlDateTimeToTimeT(const std::string& datetime)
{
	std::tm tm = {};	// 字符串结构体声明
	std::istringstream ss(datetime);			// 字符串转换为字符流
	ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");		// 字符流格式化后输入结构体
	if (ss.fail()) {			// 出错处理
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
	time_t time1 = MysqlDateTimeToTimeT(datetime1);		// 时间字符串转时间
	time_t time2 = MysqlDateTimeToTimeT(datetime2);

	double seconds_diff = std::difftime(time1, time2);		// 求时间差，并且转换为秒
	double hours_diff = seconds_diff / 3600.0;			// 秒转小时

	return hours_diff;
}

