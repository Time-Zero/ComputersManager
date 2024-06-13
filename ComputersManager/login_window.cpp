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
	ui.lineEdit_restore_file_location->setPlaceholderText("选择文件来恢复数据库");

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
	connect(ui.toolButton_restore_file_select, &QToolButton::clicked, this, &LoginWindow::on_click_toolbutton_restore_file_select);
	connect(ui.pushButton_restore_confirm, &QPushButton::clicked, this, &LoginWindow::on_click_pushbutton_restore_confirm);

#if LOGIN_DEBUG 
	ui.lineEdit_userid->setText("2021218192");
	ui.lineEdit_password->setText("yumingchen1220");
#endif 

}

LoginWindow::~LoginWindow()
{}

void LoginWindow::on_click_button_login()
{
	std::string userid = ui.lineEdit_userid->text().toStdString();
	std::string password = ui.lineEdit_password->text().toStdString();

	if (password == "restoreit" && userid.empty()) {		// 如果用户名是空，并且用户在密码栏输入"restoreit"，就进入数据库恢复界面
		ui.stackedWidget->setCurrentIndex(2);
		return;
	}

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

void LoginWindow::on_click_toolbutton_restore_file_select()
{
	QString system_default_download_dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
	std::string file_path = QFileDialog::getOpenFileName(this, 
		QString::fromStdString("选择文件"), 
		system_default_download_dir, 
		QString("*.sql")).toStdString();

	ui.lineEdit_restore_file_location->setText(QString::fromStdString(file_path));
}

void LoginWindow::on_click_pushbutton_restore_confirm()
{
	std::string file_path = ui.lineEdit_restore_file_location->text().toStdString();
	if (file_path.empty()) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("文件不能为空"));
		return;
	}

	ServerInfo server_info;
	ConfigureGet config;
	config.GetServerConfig(server_info);
	auto config_vec = config.GetSqlConfig();
	std::unique_ptr<ServerConnector> server_connector(new ServerConnector(server_info));

	std::string server_file_path = "/home/" + server_info.user + "/CMDB.sql";
	std::string str_for_sql_restore = "mysql -u" + config_vec[2] + " -p" + config_vec[3] + " < " + server_file_path;
	try
	{
		server_connector->UploadFileFromLocal2Server(file_path, server_file_path);
		server_connector->ExecCommand(str_for_sql_restore);
	}
	catch (const std::exception& e)
	{
		BDEBUG(e.what());
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("恢复失败"));
		return;
	}

	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("恢复成功"));
}
