#pragma once

#include <QWidget>
#include "ui_modify_room.h"
#include <qfile.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include "sql_service.h"
#include <future>

class ModifyRoom : public QWidget
{
	Q_OBJECT

public:
	ModifyRoom(std::string& room_name, QWidget *parent = nullptr);
	~ModifyRoom();

signals:
	void signal_modify_finish();

private:
	void closeEvent(QCloseEvent* e);
	void on_click_pushbutton_confirm();
	void on_click_pushbutton_search();

private:
	Ui::ModifyRoomClass ui;
	int row_status;
	std::string room_name;
};
