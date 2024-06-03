#include "create_room_window.h"

CreateRoomWindow::CreateRoomWindow(QWidget *parent)
	: QWidget(parent)
{
	QFile file(":/ComputersManager/create_room_window.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}
	ui.setupUi(this);
	this->setWindowModality(Qt::ApplicationModal);

	ui.lineEdit_room_name->setPlaceholderText(QString::fromStdString("输入机房名(英文+数字，首字母为字母)"));
	ui.lineEdit_cpu->setPlaceholderText(QString::fromStdString("输入CPU型号（必填）"));
	ui.lineEdit_ram->setPlaceholderText(QString::fromStdString("输入RAM大小（必填）"));
	ui.lineEdit_rom->setPlaceholderText(QString::fromStdString("输入ROM大小（必填）"));
	ui.lineEdit_gpu->setPlaceholderText(QString::fromStdString("输入GPU型号（可不填）"));
	ui.lineEdit_manager_id->setPlaceholderText(QString::fromStdString("输入管理员ID"));

	ui.lineEdit_room_name->setMaxLength(10);
	ui.lineEdit_cpu->setMaxLength(20);
	ui.lineEdit_ram->setMaxLength(15);
	ui.lineEdit_rom->setMaxLength(15);
	ui.lineEdit_gpu->setMaxLength(20);
	ui.lineEdit_manager_id->setMaxLength(10);

	ui.lineEdit_room_name->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_cpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_ram->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_rom->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_gpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_manager_id->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));

	connect(ui.pushButton_cancal, &QPushButton::clicked, this, &CreateRoomWindow::on_click_pushbutton_cancal);
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &CreateRoomWindow::on_click_pushbutton_confirm);
	connect(ui.pushButton_search_manager, &QPushButton::clicked, this, &CreateRoomWindow::on_click_pushbutton_search_manager);

#if CREATE_ROOM_DEBUG
	ui.lineEdit_room_name->setText(QString::fromStdString("rm1"));
	ui.lineEdit_cpu->setText(QString::fromStdString("i3-12100"));
	ui.lineEdit_ram->setText(QString::fromStdString("4GB"));
	ui.lineEdit_rom->setText(QString::fromStdString("512GB"));
	ui.spinBox_machine_count->setValue(30);
	ui.lineEdit_manager_id->setText(QString::fromStdString("2021218192"));
#endif
}

CreateRoomWindow::~CreateRoomWindow()
{}

void CreateRoomWindow::on_click_pushbutton_cancal()
{
	emit signal_create_room_finish();
}

void CreateRoomWindow::on_click_pushbutton_confirm()
{
	std::string room_name = ui.lineEdit_room_name->text().toStdString();
	if (room_name.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("机房名不能为空"));
		ui.lineEdit_room_name->setFocus();
		return;
	}
	else if(!::isalpha(room_name[0])) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("首字符必须为字母"));
		ui.lineEdit_room_name->setFocus();
		return;
	}
	
	std::string cpu = ui.lineEdit_cpu->text().toStdString();
	if (cpu.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("CPU型号不能为空"));
		ui.lineEdit_cpu->setFocus();
		return;
	}

	std::string ram = ui.lineEdit_ram->text().toStdString();
	if (ram.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("RAM大小不能为空"));
		ui.lineEdit_ram->setFocus();
		return;
	}

	std::string rom = ui.lineEdit_rom->text().toStdString();
	if (rom.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("ROM大小不能为空"));
		ui.lineEdit_rom->setFocus();
		return;
	}

	std::string gpu = ui.lineEdit_gpu->text().toStdString();
	unsigned int machine_num = ui.spinBox_machine_count->value();
	if (machine_num == 0) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("机器数量不能为0"));
		ui.spinBox_machine_count->setFocus();
		return;
	}

	on_click_pushbutton_search_manager();
	MachineInfo machine_info;
	machine_info.room_name = room_name;
	machine_info.cpu = cpu;
	machine_info.ram = ram;
	machine_info.rom = rom;
	machine_info.gpu = gpu;
	machine_info.machine_num = machine_num;
	machine_info.mananger_id = ui.lineEdit_manager_id->text().toStdString();
	unsigned int ret = SqlService::GetInstance().CreateRoom(machine_info);
}

void CreateRoomWindow::on_click_pushbutton_search_manager()
{
	std::string manager_id = ui.lineEdit_manager_id->text().toStdString();
	if (manager_id.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("管理员ID不能为空"));
		ui.lineEdit_manager_id->setFocus();
		return;
	}
	else if (manager_id.size() != 10) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("管理员ID格式错误"));
		ui.lineEdit_manager_id->setFocus();
		return;
	}

	UserInfo user_info;
	user_info.id = manager_id;
	SqlService::GetInstance().GetManagerInfo(user_info);
	if (user_info.name.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("不存在该管理员"));
		ui.lineEdit_manager_id->setFocus();
		return;
	}
	else if (user_info.permission < ADMIN) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("用户不能设置为管理员"));
		ui.lineEdit_manager_id->setFocus();
		return;
	}
	ui.label_manager_name->setText(QString::fromStdString(user_info.name));
}

void CreateRoomWindow::closeEvent(QCloseEvent * e)
{
	emit signal_create_room_finish();
}
