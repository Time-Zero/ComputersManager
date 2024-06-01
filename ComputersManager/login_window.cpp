#include "login_window.h"

LoginWindow::LoginWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QFile file(":/ComputersManager/login_windows.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}
	ui.lineEdit_userid->setPlaceholderText(QStringLiteral("输入学号"));
	ui.lineEdit_password->setPlaceholderText(QStringLiteral("输入密码"));
	ui.lineEdit_register_userid->setPlaceholderText(QStringLiteral("输入学号"));
	ui.lineEdit_register_username->setPlaceholderText(QStringLiteral("输入姓名"));
	ui.lineEdit_register_password->setPlaceholderText(QStringLiteral("输入密码"));
	ui.lineEdit_register_password_confirm->setPlaceholderText(QStringLiteral("确认密码"));

	ui.lineEdit_userid->setMaxLength(10);
	ui.lineEdit_password->setMaxLength(30);
	ui.lineEdit_register_userid->setMaxLength(10);
	ui.lineEdit_register_username->setMaxLength(30);
	ui.lineEdit_register_password->setMaxLength(30);
	ui.lineEdit_register_password_confirm->setMaxLength(30);

	ui.lineEdit_userid->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_register_userid->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_password_confirm->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_username->setValidator(new QRegularExpressionValidator(QRegularExpression("[\u4e00-\u9fa5]+$")));

	ui.stackedWidget->setCurrentIndex(0);
	connect(ui.pushButton_login, &QPushButton::clicked, this, &LoginWindow::on_click_button_login);
	connect(ui.pushButton_register, &QPushButton::clicked, this, &LoginWindow::on_click_button_register);
	connect(ui.pushButton_register_cancal, &QPushButton::clicked, this, &LoginWindow::on_click_button_register_cancal);
	connect(ui.pushButton_register_confirm, &QPushButton::clicked, this, &LoginWindow::on_click_button_register_confirm);

#if LOGIN_DEBUG 
	ui.lineEdit_userid->setText("2021218192");
	ui.lineEdit_password->setText("yumingchen1220");
#endif 

}

LoginWindow::~LoginWindow()
{}

void LoginWindow::on_click_button_login()
{
	ui.stackedWidget->setCurrentIndex(0);
	std::string userid = ui.lineEdit_userid->text().toStdString();
	std::string password = ui.lineEdit_password->text().toStdString();
	if (userid.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号不能为空"));
		ui.lineEdit_userid->setFocus();
		return;
	}

	if (password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码不能为空"));
		ui.lineEdit_password->setFocus();
		return;
	}

	// 异步处理
	std::future<std::string> result_from_database = std::async(std::launch::async, 
		[](std::string userid)->std::string {
		return SqlService::GetInstance().GetUserPassword(userid);
		}, userid);

	while (result_from_database.wait_for(std::chrono::milliseconds(SQL_TIMEOUT)) != std::future_status::ready) {
		QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("无法连接到后台"), QMessageBox::Ok);
		return;
	}

	std::string right_password = result_from_database.get();
	if (right_password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("用户不存在"));
		ui.lineEdit_password->setFocus();
		return;
	}
	else if (password != right_password) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码错误"));
		ui.lineEdit_password->setFocus();
		return;
	}

	emit signal_login(userid);
}

void LoginWindow::on_click_button_register()
{
#if  REGISTER_DEBUG
	ui.lineEdit_register_userid->setText("2021218191");
	ui.lineEdit_register_username->setText("雪豹");
	ui.lineEdit_register_password->setText("ilovedingzhen");
	ui.lineEdit_register_password_confirm->setText("ilovedingzhen");
#endif //  REGISTER_DEBUG

	ui.stackedWidget->setCurrentIndex(1);
}

void LoginWindow::on_click_button_register_confirm()
{
	std::string userid = ui.lineEdit_register_userid->text().toStdString();
	if (userid.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号不能为空"));
		ui.lineEdit_register_userid->setFocus();
		return;
	}
	else if (userid.size() < 10) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号格式错误"));
		ui.lineEdit_register_userid->setFocus();
		return;
	}

	std::string username = ui.lineEdit_register_username->text().toStdString();
	if (username.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("姓名不能为空"));
		ui.lineEdit_register_username->setFocus();
		return;
	}

	std::string password = ui.lineEdit_register_password->text().toStdString();
	if (password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码不能为空"));
		ui.lineEdit_register_password->setFocus();
		return;
	}
	else if (password.size() < 8) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码应大于8位"));
		ui.lineEdit_register_username->setFocus();
		return;
	}

	std::string password_confirm = ui.lineEdit_register_password_confirm->text().toStdString();
	if (password != password_confirm) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("两次输入密码不一致"));
		ui.lineEdit_register_password_confirm->setFocus();
		return;
	}

	UserInfo user_info;
	user_info.id = userid;
	user_info.name = username;
	user_info.password = password;

	std::future<unsigned int> fut_ret = std::async(std::launch::async, [&]() {
		return SqlService::GetInstance().Register(user_info);
		});

	while (fut_ret.wait_for(std::chrono::milliseconds(SQL_TIMEOUT)) != std::future_status::ready) {
		QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("无法连接到后台"));
		return;
	}


	unsigned int ret = fut_ret.get();
	if (ret == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("注册成功,点击确认返回登录"), QMessageBox::Ok);
		on_click_button_register_cancal();
	}
	else if (ret == 1) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("用户已经被注册"), QMessageBox::Ok);
		ui.lineEdit_register_userid->setFocus();
	}
}


void LoginWindow::on_click_button_register_cancal()
{
	ui.stackedWidget->setCurrentIndex(0);
	ui.lineEdit_register_userid->clear();
	ui.lineEdit_register_username->clear();
	ui.lineEdit_register_password->clear();
	ui.lineEdit_register_password_confirm->clear();
}
