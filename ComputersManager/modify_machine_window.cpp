#include "modify_machine_window.h"

ModifyMachineWindow::ModifyMachineWindow(std::string room_name, std::string machine_id, QWidget *parent)
	: QWidget(parent),
	room_name_(room_name),
	machine_id_(machine_id)
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
	}
	else if (ret == 2) {				// 有人使用，不允许停用
		ui.comboBox_status->setEnabled(false);
	}
	else{
		ui.comboBox_status->setCurrentIndex(ret);
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

}
