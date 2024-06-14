﻿#include "add_machine_winodw.h"

/// @brief 构造函数
/// @param room_name 要添加机器的房间名 
/// @param parent 
AddMachineWinodw::AddMachineWinodw(std::string room_name, QWidget *parent)
	: QWidget(parent),
	room_name_(room_name)	
{
	// 读取样式表
	QFile file(":/ComputersManager/login_windows.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	ui.setupUi(this);
	this->setWindowModality(Qt::ApplicationModal);		// 设置本窗口阻塞其他窗口

	// 输入框提示
	ui.lineEdit_cpu->setPlaceholderText(QString::fromStdString("CPU（必填）"));
	ui.lineEdit_ram->setPlaceholderText(QString::fromStdString("RAM（必填）"));
	ui.lineEdit_rom->setPlaceholderText(QString::fromStdString("ROM（必填）"));
	ui.lineEdit_gpu->setPlaceholderText(QString::fromStdString("GPU（选填）"));

	// 输入框最大长度限制
	ui.lineEdit_cpu->setMaxLength(20);
	ui.lineEdit_ram->setMaxLength(15);
	ui.lineEdit_rom->setMaxLength(15);
	ui.lineEdit_gpu->setMaxLength(20);

	// 设置输入框过滤器
	ui.lineEdit_cpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9-]+$")));
	ui.lineEdit_ram->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_rom->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
	ui.lineEdit_gpu->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9-]+$")));

	// 信号与槽的绑定
	connect(ui.pushButton_cancal, &QPushButton::clicked, this, &AddMachineWinodw::on_click_pushbutton_cancal);
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &AddMachineWinodw::on_click_pushbutton_confirm);

#if MODIFY_MACHINE_DEBUG
	ui.lineEdit_cpu->setText(QString::fromStdString("i3-12100"));
	ui.lineEdit_ram->setText(QString::fromStdString("4GB"));
	ui.lineEdit_rom->setText(QString::fromStdString("1TB"));
	ui.lineEdit_gpu->setText(QString::fromStdString("RTX3070"));
#endif
}

AddMachineWinodw::~AddMachineWinodw()
{}

/// @brief 重写的关闭事件
/// @param e 
void AddMachineWinodw::closeEvent(QCloseEvent * e)
{
	on_click_pushbutton_cancal();	// 关闭窗口等于点击取消按钮
}

/// @brief 取消按钮的槽函数
void AddMachineWinodw::on_click_pushbutton_cancal()
{
	emit signal_modify_finish();	// 发送信号让主窗口关闭本窗口
}

/// @brief 确认按钮的槽函数
void AddMachineWinodw::on_click_pushbutton_confirm()
{
	// 获取cpu型号
	std::string cpu = ui.lineEdit_cpu->text().toStdString();
	if (cpu.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("CPU型号不能为空"));
		ui.lineEdit_cpu->setFocus();
		return;
	}

	// 获取ram型号
	std::string ram = ui.lineEdit_ram->text().toStdString();
	if (ram.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("RAM大小不能为空"));
		ui.lineEdit_ram->setFocus();
		return;
	}

	// 获取rom型号
	std::string rom = ui.lineEdit_rom->text().toStdString();
	if (rom.empty()) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("ROM大小不能为空"));
		ui.lineEdit_rom->setFocus();
		return;
	}
	
	// 获取gpu信号，可以留空
	std::string gpu = ui.lineEdit_gpu->text().toStdString();

	// 用一个结构体把这些都封装起来
	Machine machine;
	machine.cpu = cpu;
	machine.ram = ram;
	machine.rom = rom;
	machine.gpu = gpu;
	
	// 执行sql语句
	unsigned int ret = SqlService::GetInstance().AddMachine(room_name_, machine);
	if (ret == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("添加成功"));
		on_click_pushbutton_cancal();
	}
	else {
		QMessageBox::information(this, QStringLiteral("失败"), QStringLiteral("未知错误"));
	}
}
