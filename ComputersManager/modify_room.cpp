#include "modify_room.h"

ModifyRoom::ModifyRoom(std::string& room_name, unsigned int permission, QWidget *parent)
	: QWidget(parent)
{

	//TODO:完成机房修改部分
	ui.setupUi(this);
	QFile file(":/ComputersManager/change_userinfo.qss");
	if (file.open(QFile::ReadOnly)) {
		this->setStyleSheet(file.readAll());
		file.close();
	}
	
	ui.lineEdit_room_name->setPlaceholderText(QString::fromStdString(room_name));
	ui.lineEdit_room_name->setEnabled(false);

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
