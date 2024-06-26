﻿#include "rent_machine_window.h"

RentMachineWindow::RentMachineWindow(std::string room_name, std::string machine_id, QWidget *parent)
	: QWidget(parent),
	room_name_(room_name),
	machine_id_(machine_id)
{
	auto fut_ret = std::async(std::launch::async, [&]() {
		return SqlService::GetInstance().GetMachineUser(room_name_, machine_id_);
		});


	ui.setupUi(this);
	this->setWindowModality(Qt::ApplicationModal);
	QFile file(":/ComputersManager/login_windows.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}
	this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

	ui.label_pay->resize(468, 220);
	QPixmap pix(":/picture/images/pay_code.jpg");
	QPixmap dest = pix.scaled(ui.label_pay->size(), Qt::KeepAspectRatio);
	ui.label_pay->setPixmap(dest);


	ui.lineEdit_user_id->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_user_id->setPlaceholderText(QString::fromStdString("输入学号"));
	ui.lineEdit_user_id->setMaxLength(10);

	connect(ui.pushButton_find_user, &QPushButton::clicked, this, &RentMachineWindow::on_click_pushbutton_find_user);
	connect(ui.pushButton_rent, &QPushButton::clicked, this, &RentMachineWindow::on_click_pushbutton_rent);
	connect(ui.pushButton_end_rent, &QPushButton::clicked, this, &RentMachineWindow::on_click_pushbutton_end_rent);
	connect(ui.pushButton_pay_ok, &QPushButton::clicked, this, [=]() {emit signal_rent_finish(); });

	std::vector<std::string> machine_user_info = fut_ret.get();
	if (machine_user_info[0] == "0") {			//停用就直接提示然后退出
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("该电脑处于停用状态"));
		emit signal_rent_finish();
	}
	else if (machine_user_info[0] == "1") {		// 空闲切换到上机页面
		ui.stackedWidget->setCurrentIndex(PAGE_NO_USE);
	}
	else if (machine_user_info[0] == "2") {		// 使用的话切换到使用者信息页面
		ui.stackedWidget->setCurrentIndex(PAGE_ON_USE);
		auto ret = SqlService::GetInstance().GetRentInfo(room_name_, machine_id_);
		ui.label_user_id->setText(QString::fromStdString(ret[0]));
		ui.label_username->setText(QString::fromStdString(ret[1]));
		ui.label_sdate->setText(QString::fromStdString(ret[2]));
		ui.label_fee->setText(QString::fromStdString(ret[3]));
	}
	
}

RentMachineWindow::~RentMachineWindow()
{}


void RentMachineWindow::closeEvent(QCloseEvent * e)
{
	emit signal_rent_finish();
}

void RentMachineWindow::on_click_pushbutton_find_user()
{
	std::string user_id = ui.lineEdit_user_id->text().toStdString();
	if (user_id.empty()) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请输入学号"));
		ui.lineEdit_user_id->setFocus();
		return;
	}
	else if(user_id.length() != 10){
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("学号格式存在问题"));
		ui.lineEdit_user_id->setFocus();
		return;
	}
	
	std::string user_name = SqlService::GetInstance().GetUserName(user_id);
	if (user_name.empty()) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("用户不存在"));
		ui.lineEdit_user_id->setFocus();
		return;
	}

	ui.label_user_name->setText(QString::fromStdString(user_name));
}

void RentMachineWindow::on_click_pushbutton_rent()
{
	on_click_pushbutton_find_user();
	std::string user_id = ui.lineEdit_user_id->text().toStdString();
	int ret = SqlService::GetInstance().CheckSomeOneIsRent(user_id);
	
	if (ret == 1) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("此人已经上机"));
		return;
	}
	

	ret = SqlService::GetInstance().RentMachine(room_name_, machine_id_, user_id);
	if (ret == -1) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("未知错误"));
		return;
	}

	QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("上机成功"));
	emit signal_rent_finish();
}

void RentMachineWindow::on_click_pushbutton_end_rent()
{
	double fee = SqlService::GetInstance().EndRent(room_name_, machine_id_);
	if (fee < 0) {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("未知错误"));
		return;
	}
	
	ui.stackedWidget->setCurrentIndex(PAGE_PAY);

}
