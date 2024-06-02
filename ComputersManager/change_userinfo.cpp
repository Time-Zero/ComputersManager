#include "change_userinfo.h"

ChangeUserInfo::ChangeUserInfo(std::string& userid, unsigned int permission, QWidget* parent)
	: QWidget(parent)
{
	UserInfo user_info;
	user_info.id = userid;
	std::future<void> fut = std::async(std::launch::async, [&]() {
		SqlService::GetInstance().GetUserInfo(user_info);
		});
	
	ui.setupUi(this);
	QFile file(":/ComputersManager/change_userinfo.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	this->setWindowModality(Qt::ApplicationModal);
	ui.lineEdit_userid->setText(QString::fromStdString(userid));
	ui.lineEdit_userid->setEnabled(false);
	ui.lineEdit_username->setValidator(new QRegularExpressionValidator(QRegularExpression("[\u4e00-\u9fa5]+$")));
	ui.lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_username->setPlaceholderText(QString::fromStdString("输入新姓名"));
	ui.lineEdit_password->setPlaceholderText(QString::fromStdString("输入新密码"));
	ui.lineEdit_username->setMaxLength(30);
	ui.lineEdit_password->setMaxLength(30);

	if (permission > ADMIN) {
		ui.comboBox_permission->addItem(QString::fromStdString("用户"));
		ui.comboBox_permission->addItem(QString::fromStdString("管理员"));
	}
	else {
		ui.comboBox_permission->addItem(QString::fromStdString("用户"));
	}

	ui.comboBox_order->addItem(QString::fromStdString("否"));
	ui.comboBox_order->addItem(QString::fromStdString("是"));

	fut.get();
	ui.lineEdit_username->setText(QString::fromStdString(user_info.name));
	ui.lineEdit_password->setText(QString::fromStdString(user_info.password));
	ui.comboBox_permission->setCurrentIndex(user_info.permission - 1);
	ui.comboBox_order->setCurrentIndex(user_info.order);

	connect(ui.pushButton_cancal, &QPushButton::clicked, this, [&]() {
		emit signal_change_finish();
		});
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &ChangeUserInfo::on_click_button_confirm);
}

ChangeUserInfo::~ChangeUserInfo()
{}

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

	UserInfo user_info;
	user_info.id = ui.lineEdit_userid->text().toStdString();
	user_info.name = user_name;
	user_info.password = user_password;
	user_info.permission = user_permission;
	user_info.order = user_order;
	unsigned int ret = SqlService::GetInstance().ChangInfo(user_info);
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
