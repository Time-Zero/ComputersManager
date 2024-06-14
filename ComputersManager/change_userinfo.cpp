#include "change_userinfo.h"

/// @brief 修改用户信息窗口的构造函数
/// @param userid 要修改的用户id
/// @param permission 你的权限
/// @param parent 
ChangeUserInfo::ChangeUserInfo(std::string& userid, unsigned int permission, QWidget* parent)
	: QWidget(parent)
{
	// 使用结构体封装要查询的用户信息
	UserInfo user_info;
	user_info.id = userid;
	std::future<void> fut = std::async(std::launch::async, [&]() {		// 获取用户的信息
		SqlService::GetInstance().GetUserInfo(user_info);
		});
	
	// 样式表读取
	ui.setupUi(this);
	QFile file(":/ComputersManager/change_userinfo.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	this->setWindowModality(Qt::ApplicationModal);		// 阻塞其他窗口
	ui.lineEdit_userid->setText(QString::fromStdString(userid));		// 设置用户id
	ui.lineEdit_userid->setEnabled(false);
	ui.lineEdit_username->setValidator(new QRegularExpressionValidator(QRegularExpression("[\u4e00-\u9fa5]+$")));
	ui.lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_username->setPlaceholderText(QString::fromStdString("输入新姓名"));
	ui.lineEdit_password->setPlaceholderText(QString::fromStdString("输入新密码"));
	ui.lineEdit_username->setMaxLength(30);
	ui.lineEdit_password->setMaxLength(30);

	// 修改者鉴权，只有超级管理员能设置用户权限为管理员
	if (permission > SADMIN) {
		ui.comboBox_permission->addItem(QString::fromStdString("用户"));
		ui.comboBox_permission->addItem(QString::fromStdString("管理员"));
	}
	else {
		ui.comboBox_permission->addItem(QString::fromStdString("用户"));
	}

	// 是否能够预约，这没啥用，预约功能我没写
	ui.comboBox_order->addItem(QString::fromStdString("否"));
	ui.comboBox_order->addItem(QString::fromStdString("是"));

	fut.get();		// 获取从数据库返回的用户信息
	ui.lineEdit_username->setText(QString::fromStdString(user_info.name));		// 用户名
	ui.lineEdit_password->setText(QString::fromStdString(user_info.password));		// 用户密码
	ui.comboBox_permission->setCurrentIndex(user_info.permission - 1);			// 用户权限
	ui.comboBox_order->setCurrentIndex(user_info.order);		// 能不能预约

	connect(ui.pushButton_cancal, &QPushButton::clicked, this, [&]() {		// 信号和槽函数绑定，按一下取消就发送关闭这个窗口的指令
		emit signal_change_finish();
		});
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &ChangeUserInfo::on_click_button_confirm);	// 绑定确认按钮和确认键
}

ChangeUserInfo::~ChangeUserInfo()
{}

/// @brief 确认按钮的槽函数
void ChangeUserInfo::on_click_button_confirm()
{
	std::string user_name = ui.lineEdit_username->text().toStdString();
	if (user_name.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("姓名不能为空"));
		return;
	}

	std::string user_password = ui.lineEdit_password->text().toStdString();
	if (user_password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码不能为空"));
		return;
	}
	
	unsigned int user_permission = ui.comboBox_permission->currentIndex() + 1;
	unsigned int user_order = ui.comboBox_order->currentIndex();

	// 把修改后的用户信息封装一下
	UserInfo user_info;
	user_info.id = ui.lineEdit_userid->text().toStdString();
	user_info.name = user_name;
	user_info.password = user_password;
	user_info.permission = user_permission;
	user_info.order = user_order;
	unsigned int ret = SqlService::GetInstance().ChangInfo(user_info);		// 通过sql语句发送到数据库
	if (ret == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改成功"));
		emit signal_change_finish();
	}
	else {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("未知错误"));
	}
}

void ChangeUserInfo::closeEvent(QCloseEvent* e)
{
	emit signal_change_finish();
}
