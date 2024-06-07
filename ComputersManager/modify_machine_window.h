#pragma once

#include <QWidget>
#include "ui_modify_machine_window.h"
#include <qfile.h>
#include <qvalidator.h>
#include <qregularexpression.h>
#include "sql_service.h"
#include <qmessagebox.h>

class ModifyMachineWindow : public QWidget
{
	Q_OBJECT

public:
	ModifyMachineWindow(std::string room_name,std::string machine_id ,QWidget *parent = nullptr);
	~ModifyMachineWindow();
	
signals:
	void signal_modify_finish();

private:
	void closeEvent(QCloseEvent* e);
	void on_click_pushbutton_cancal();
	void on_click_pushbutton_confirm();

private:
	Ui::ModifyMachineWindowClass ui;
	std::string room_name_;
	std::string machine_id_;
	int row_status;
};
