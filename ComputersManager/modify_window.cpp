#include "modify_window.h"

ModifyWindow::ModifyWindow(QWidget* parent, std::string userid)
	: QWidget(parent),
	userid_(userid)
{
	ui.setupUi(this);
	
	QFile file(":/ComputersManager/modify_window.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	this->setWindowModality(Qt::ApplicationModal);
	ui.lineEdit_username->setPlaceholderText(QStringLiteral("姓名"));
	ui.lineEdit_password->setPlaceholderText(QStringLiteral("输入新密码"));
	ui.lineEdit_password_confirm->setPlaceholderText(QStringLiteral("重复新密码"));

	ui.lineEdit_username->setValidator(new QRegularExpressionValidator(QRegularExpression("[\u4e00-\u9fa5]+$")));
	ui.lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_password_confirm->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));

	
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &ModifyWindow::on_click_button_confirm);
	connect(ui.pushButton_cancal, &QPushButton::clicked, this, &ModifyWindow::on_click_button_cancal);


#if MODEIFY_DEBUG
	ui.lineEdit_username->setText(QString::fromStdString("王源"));
	ui.lineEdit_password->setText(QString::fromStdString("ilovezhonghua"));
	ui.lineEdit_password_confirm->setText(QString::fromStdString("ilovezhonghua"));
#endif
}

ModifyWindow::~ModifyWindow()
{
}

void ModifyWindow::on_click_button_cancal()
{
	emit signal_modify_finish();
}

void ModifyWindow::on_click_button_confirm()
{
	std::string user_name = ui.lineEdit_username->text().toStdString();
	if (user_name.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("姓名不能为空"), QMessageBox::Ok);
		return;
	}

	std::string user_password = ui.lineEdit_password->text().toStdString();
	if (user_password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码不能为空"), QMessageBox::Ok);
		return;
	}

	std::string user_password_confirm = ui.lineEdit_password_confirm->text().toStdString();
	if (user_password != user_password_confirm) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("前后密码不一致"), QMessageBox::Ok);
		return;
	}

	UserInfo user_info;
	user_info.id = userid_;
	user_info.name = user_name;
	user_info.password = user_password;

	std::future<unsigned int> fut_ret = std::async(std::launch::async, [&]() { 
		return SqlService::GetInstance().ModifyInfo(user_info);
	});
	
	while (fut_ret.wait_for(std::chrono::microseconds(SQL_TIMEOUT)) != std::future_status::ready) {
		QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("与后端断开连接"), QMessageBox::Ok);
		return;
	}

	if (fut_ret.get() == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改成功"), QMessageBox::Ok);
		emit signal_modify_finish();
	}
	else {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("修改失败，未知错误"), QMessageBox::Ok);
	}
}

void ModifyWindow::closeEvent(QCloseEvent* e)
{
	on_click_button_cancal();
}
