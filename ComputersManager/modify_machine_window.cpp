#include "modify_machine_window.h"

ModifyMachineWindow::ModifyMachineWindow(std::string room_name, std::string machine_id, QWidget *parent)
	: QWidget(parent),
	room_name_(room_name),
	machine_id_(machine_id),
	row_status(-1)
{

	ui.setupUi(this);
	this->setWindowModality(Qt::ApplicationModal);
	QFile file(":/ComputersManager/change_userinfo.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	ui.lineEdit_cpu->setPlaceholderText(QString::fromStdString("CPU"));
	ui.lineEdit_ram->setPlaceholderText(QString::fromStdString("RAM"));
	ui.lineEdit_rom->setPlaceholderText(QString::fromStdString("ROM"));
	ui.lineEdit_gpu->setPlaceholderText(QString::fromStdString("GPU"));

	ui.lineEdit_cpu->setMaxLength(20);
	ui.lineEdit_ram->setMaxLength(15);
	ui.lineEdit_rom->setMaxLength(15);
	ui.lineEdit_gpu->setMaxLength(20);

	ui.lineEdit_cpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9-]+$")));
	ui.lineEdit_ram->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_rom->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_gpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9-]+$")));

	connect(ui.pushButton_cancal, &QPushButton::clicked, this, &ModifyMachineWindow::on_click_pushbutton_cancal);
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &ModifyMachineWindow::on_click_pushbutton_confirm);

	int ret = SqlService::GetInstance().GetMachineStatus(room_name_, machine_id_);
	if (ret < 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("未知错误"));
		on_click_pushbutton_cancal();			// 出现错误，不允许再进行后续的修改操作
	}
	else if (ret == 2) {				// 有人使用，不允许停用
		ui.comboBox_status->setEnabled(false);
	}
	else{
		ui.comboBox_status->setCurrentIndex(ret);
		row_status = ret;
	}
}

ModifyMachineWindow::~ModifyMachineWindow()
{}

void ModifyMachineWindow::closeEvent(QCloseEvent* e)
{
	on_click_pushbutton_cancal();
}

void ModifyMachineWindow::on_click_pushbutton_cancal()
{
	emit signal_modify_finish();
}

void ModifyMachineWindow::on_click_pushbutton_confirm()
{

	std::string cpu = ui.lineEdit_cpu->text().toStdString();
	std::string ram = ui.lineEdit_ram->text().toStdString();
	std::string rom = ui.lineEdit_rom->text().toStdString();
	std::string gpu = ui.lineEdit_gpu->text().toStdString();
	int new_status = -1;

	if (row_status != -1) {		//也就是row_status被赋值为了0或者1，也就是可以操作停用键
		// 如果状态被改变了，就把新状态传出去，否则还是-1
		new_status = ui.comboBox_status->currentIndex() != row_status ? ui.comboBox_status->currentIndex() : -1;
	}

	if (cpu.empty() && ram.empty() && rom.empty() && gpu.empty() && new_status == -1)		//全部为空，也就是不修改任何东西，直接返回就行了
		return;

	Machine machine;
	machine.id = std::atoi(machine_id_.c_str());
	machine.cpu = cpu;
	machine.ram = ram;
	machine.rom = rom;
	machine.gpu = gpu;
	machine.status = new_status;
	int ret = SqlService::GetInstance().ModifyMachineInfo(machine, room_name_);
	if (ret == -1) {
		QMessageBox::information(this, QStringLiteral("修改失败"), QStringLiteral("未知错误"));
	}
	else {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改成功"));
		on_click_pushbutton_cancal();
	}
}
