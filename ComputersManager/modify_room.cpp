#include "modify_room.h"

ModifyRoom::ModifyRoom(std::string& room_name, QWidget *parent)
	: QWidget(parent)
	, room_name(room_name)
{
	auto fut_room_info = std::async(std::launch::async, [&]() {
		return SqlService::GetInstance().GetRoomInfo(room_name);
		});

	//TODO:完成机房修改部分
	ui.setupUi(this);
	QFile file(":/ComputersManager/change_userinfo.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}

	std::vector<std::string> room_info = fut_room_info.get();

	//从数据库中获取状态然后把修改框的状态设置为当前状态 
	row_status = std::atoi(room_info[0].c_str());
	ui.comboBox->setCurrentIndex(row_status);

	ui.lineEdit_room_name->setPlaceholderText(QString::fromStdString(room_name));
	ui.lineEdit_room_name->setEnabled(false);
	ui.lineEdit_manager_id->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
	ui.lineEdit_manager_id->setPlaceholderText(QString::fromStdString("输入新管理员ID"));

	connect(ui.pushButton_search, &QPushButton::clicked, this, &ModifyRoom::on_click_pushbutton_search);
	connect(ui.pushButton_confirm, &QPushButton::clicked, this, &ModifyRoom::on_click_pushbutton_confirm);
	connect(ui.pushButton_cancal, &QPushButton::clicked, this, [&]() {
		emit signal_modify_finish();
		});
}

ModifyRoom::~ModifyRoom()
{}

void ModifyRoom::closeEvent(QCloseEvent * e)
{
	emit signal_modify_finish();
}

void ModifyRoom::on_click_pushbutton_confirm()
{
	std::string manager_id = ui.lineEdit_manager_id->text().toStdString();
	double fees = ui.doubleSpinBox_fees->value();
	int status = ui.comboBox->currentIndex();
	if (manager_id.empty() && status == row_status && fees == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请做出修改后再提交"));
		return;
	}

	if (status == row_status) status = -1;			//如果没有修改状态，状态就是-1
	
	// 如果设置了新的管理员id，就查找
	if(!manager_id.empty())
		on_click_pushbutton_search();

	int ret = SqlService::GetInstance().ModifyRoomInfo(room_name, { status, manager_id }, fees);
	
	if (ret == -1) {
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("修改失败，未知错误"));
	}
	else if (ret == 1) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("机房还有人使用，无法停用"));
	}
	else {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改成功"));
		emit signal_modify_finish();
	}
}

void ModifyRoom::on_click_pushbutton_search()
{
	std::string manager_id = ui.lineEdit_manager_id->text().toStdString();

	if (manager_id.size() == 10) {
		UserInfo user_info;
		user_info.id = manager_id;
		SqlService::GetInstance().GetManagerInfo(user_info);
		if (user_info.name.empty()) {
			QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("查无此人"));
			ui.lineEdit_manager_id->setFocus();
			return;
		}
		else if (user_info.permission < ADMIN) {
			QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("普通用户不能设置为管理员"));
			ui.lineEdit_manager_id->setFocus();
			return;
		}
		else {
			ui.label_manager_name->setText(QString::fromStdString(user_info.name));
		}
	}
	else {
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("管理员ID格式错误"));
		ui.lineEdit_manager_id->setFocus();
		return;
	}
}
