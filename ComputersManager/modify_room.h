#pragma once

#include <QWidget>
#include "ui_modify_room.h"
#include <qfile.h>

class ModifyRoom : public QWidget
{
	Q_OBJECT

public:
	ModifyRoom(std::string& room_name, unsigned int permission,QWidget *parent = nullptr);
	~ModifyRoom();

signals:
	void signal_modify_finish();

private:
	void closeEvent(QCloseEvent* e);

private:
	Ui::ModifyRoomClass ui;
};
