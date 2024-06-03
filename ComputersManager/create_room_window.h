#pragma once

#include <QWidget>
#include "ui_create_room_window.h"
#include <qvalidator.h>
#include <qregularexpression.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "sql_service.h"

class CreateRoomWindow : public QWidget
{
	Q_OBJECT

public:
	CreateRoomWindow(QWidget *parent = nullptr);
	~CreateRoomWindow();

signals:
	void signal_create_room_finish();

private:
	void on_click_pushbutton_cancal();
	void on_click_pushbutton_confirm();
	void on_click_pushbutton_search_manager();
	void closeEvent(QCloseEvent* e);
private:
	Ui::CreateRoomWindowClass ui;
};
