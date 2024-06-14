#include "login_window.h"

/// @brief 构造函数
/// @param parent 
LoginWindow::LoginWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QFile file(":/ComputersManager/login_windows.qss");			// 样式表读取
	if (file.open(QFile::ReadOnly)) {							// 样式设置
		this->setStyleSheet(file.readAll());
		file.close();
	}

	// lineEdit输入提示文字设置
	ui.lineEdit_userid->setPlaceholderText(QStringLiteral("输入学号"));
	ui.lineEdit_password->setPlaceholderText(QStringLiteral("输入密码"));
	ui.lineEdit_register_userid->setPlaceholderText(QStringLiteral("输入学号"));
	ui.lineEdit_register_username->setPlaceholderText(QStringLiteral("输入姓名"));
	ui.lineEdit_register_password->setPlaceholderText(QStringLiteral("输入密码"));
	ui.lineEdit_register_password_confirm->setPlaceholderText(QStringLiteral("确认密码"));
	ui.lineEdit_restore_file_location->setPlaceholderText("选择文件来恢复数据库");

	// 设置输入栏的最大长度
	ui.lineEdit_userid->setMaxLength(10);
	ui.lineEdit_password->setMaxLength(30);
	ui.lineEdit_register_userid->setMaxLength(10);
	ui.lineEdit_register_username->setMaxLength(30);
	ui.lineEdit_register_password->setMaxLength(30);
	ui.lineEdit_register_password_confirm->setMaxLength(30);

	// 设置输入栏的过滤器，让其只允许输入指定的字符
	ui.lineEdit_userid->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_register_userid->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_password->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_password_confirm->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_register_username->setValidator(new QRegularExpressionValidator(QRegularExpression("[\u4e00-\u9fa5]+$")));

	
	ui.stackedWidget->setCurrentIndex(0);		// 设置进入的第一页是登录页
	
	// 按钮的信号和对应的槽函数绑定
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

// 析构函数
LoginWindow::~LoginWindow()
{}

/// @brief 登录按钮的槽函数
void LoginWindow::on_click_button_login()
{
	std::string userid = ui.lineEdit_userid->text().toStdString();				// 从输入栏读取学号
	std::string password = ui.lineEdit_password->text().toStdString();			// 从输入栏读取密码

	if (password == "restoreit" && userid.empty()) {		// 如果用户名是空，并且用户在密码栏输入"restoreit"，就进入数据库恢复界面
		ui.stackedWidget->setCurrentIndex(2);
		return;
	}

	// 正常登录不允许用户名和密码为空
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

	// 异步处理，从数据库获取用户名对应的密码
	std::future<std::string> result_from_database = std::async(std::launch::async, 
		[](std::string userid)->std::string {
		return SqlService::GetInstance().GetUserPassword(userid);
		}, userid);

	// 一个延时，看看数据库能不能在指定时间返回结果，如果不能就提示断开和后台的链接
	while (result_from_database.wait_for(std::chrono::milliseconds(SQL_TIMEOUT)) != std::future_status::ready) {
		QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("无法连接到后台"), QMessageBox::Ok);
		return;
	}

	std::string right_password = result_from_database.get();		// 从future中get返回值
	if (right_password.empty()) {			// 如果返回值是空的，则不存在这个用户
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("用户不存在"));
		ui.lineEdit_password->setFocus();
		return;
	}
	else if (password != right_password) {		// 如果返回值和输入值不同，则报错密码错误
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码错误"));
		ui.lineEdit_password->setFocus();
		return;
	}

	emit signal_login(userid);			// 发送信号给主窗口，让主窗口释放掉登录窗口的内存并且从隐藏状态中显示
}

/// @brief 注册按钮的槽函数
void LoginWindow::on_click_button_register()
{
#if  REGISTER_DEBUG
	ui.lineEdit_register_userid->setText("2021218191");
	ui.lineEdit_register_username->setText("雪豹");
	ui.lineEdit_register_password->setText("ilovedingzhen");
	ui.lineEdit_register_password_confirm->setText("ilovedingzhen");
#endif //  REGISTER_DEBUG

	ui.stackedWidget->setCurrentIndex(1);			// 如果按下注册按钮就把statckpage切换到注册页面
}

/// @brief 确认注册按钮的槽函数
void LoginWindow::on_click_button_register_confirm()
{
	std::string userid = ui.lineEdit_register_userid->text().toStdString();		// 从linedit中获取值
	if (userid.empty()) {			// 如果输入框中为空
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号不能为空"));
		ui.lineEdit_register_userid->setFocus();
		return;
	}
	else if (userid.size() < 10) {  /// 如果学号长度不是十位
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号格式错误"));
		ui.lineEdit_register_userid->setFocus();
		return;
	}

	std::string username = ui.lineEdit_register_username->text().toStdString();		// 从lineedit中获取用户名
	if (username.empty()) {				// 如果用户名为空
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("姓名不能为空"));
		ui.lineEdit_register_username->setFocus();
		return;
	}

	std::string password = ui.lineEdit_register_password->text().toStdString();			// 获取用户输入的密码
	if (password.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码不能为空"));
		ui.lineEdit_register_password->setFocus();
		return;
	}
	else if (password.size() < 8) {			// 密码应该大于8位
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("密码应大于8位"));
		ui.lineEdit_register_username->setFocus();
		return;
	}

	// 比较密码和确认密码框中内容是否一致
	std::string password_confirm = ui.lineEdit_register_password_confirm->text().toStdString();
	if (password != password_confirm) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("两次输入密码不一致"));
		ui.lineEdit_register_password_confirm->setFocus();
		return;
	}

	// 把学号，姓名，密码封装到一个结构体中
	UserInfo user_info;
	user_info.id = userid;
	user_info.name = username;
	user_info.password = password;

	// sql异步执行
	std::future<unsigned int> fut_ret = std::async(std::launch::async, [&]() {
		return SqlService::GetInstance().Register(user_info);
		});

	while (fut_ret.wait_for(std::chrono::milliseconds(SQL_TIMEOUT)) != std::future_status::ready) {
		QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("无法连接到后台"));
		return;
	}
	
	// 获取sql执行结果
	unsigned int ret = fut_ret.get();
	if (ret == 0) {		// 返回0，则注册成功
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("注册成功,点击确认返回登录"), QMessageBox::Ok);
		on_click_button_register_cancal();
	}
	else if (ret == 1) {		// 返回1，用户已经被注册了
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("用户已经被注册"), QMessageBox::Ok);
		ui.lineEdit_register_userid->setFocus();
	}
}


void LoginWindow::on_click_button_register_cancal()		// 注册取消按钮
{
	ui.stackedWidget->setCurrentIndex(0);				// 取消注册就切换到登录页面
	ui.lineEdit_register_userid->clear();				// 把注册页的学号输入框清空
	ui.lineEdit_register_username->clear();				// 姓名清空
	ui.lineEdit_register_password->clear();				// 密码清空
	ui.lineEdit_register_password_confirm->clear();		// 确认密码清空
}

void LoginWindow::on_click_toolbutton_restore_file_select()		// 恢复页面的选择恢复的sql文件按钮
{
	QString system_default_download_dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);	// 获取系统的标准下载地址
	std::string file_path = QFileDialog::getOpenFileName(this,					// 调出一个文件选择窗口，并且设置默认路径在下载地址那里
		QString::fromStdString("选择文件"), 
		system_default_download_dir, 
		QString("*.sql")).toStdString();

	ui.lineEdit_restore_file_location->setText(QString::fromStdString(file_path));		// 把选择的路径结果放到lineedit中
}

void LoginWindow::on_click_pushbutton_restore_confirm()		// 确认恢复按钮
{
	std::string file_path = ui.lineEdit_restore_file_location->text().toStdString();		// 从lineedit中获取用户选择的备份文件的地址
	if (file_path.empty()) {			// 如果文件选择框为空，则提示
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("文件不能为空"));
		return;
	}

	// 从配置文件中读取服务器和数据库的信息
	ServerInfo server_info;
	ConfigureGet config;
	config.GetServerConfig(server_info);
	auto config_vec = config.GetSqlConfig();
	std::unique_ptr<ServerConnector> server_connector(new ServerConnector(server_info));

	std::string server_file_path = "/home/" + server_info.user + "/CMDB.sql";			// 文件在数据库中的保存地址
	std::string str_for_sql_restore = "mysql -u" + config_vec[2] + " -p" + config_vec[3] + " < " + server_file_path;		// 组合恢复命令
	try
	{
		server_connector->UploadFileFromLocal2Server(file_path, server_file_path);			// 把本地备份文件上传到数据库
		server_connector->ExecCommand(str_for_sql_restore);									// 执行恢复指令
	}
	catch (const std::exception& e)
	{
		BDEBUG(e.what());
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("恢复失败"));
		return;
	}

	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("恢复成功"));
}
