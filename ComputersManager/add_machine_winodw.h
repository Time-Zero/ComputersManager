#pragma once

#include "sql_service.h"
#include "ui_add_machine_winodw.h"
#include <qfile.h>
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QWidget>
#include <qmessagebox.h>

class AddMachineWinodw : public QWidget
{
	Q_OBJECT

public:
	AddMachineWinodw(std::string room_name, QWidget *parent = nullptr);
	~AddMachineWinodw();

signals:
	void signal_modify_finish();

private:
	void closeEvent(QCloseEvent* e);
	void on_click_pushbutton_cancal();
	void on_click_pushbutton_confirm();
	
private:
	Ui::AddMachineWinodwClass ui;
	std::string room_name_;
};
