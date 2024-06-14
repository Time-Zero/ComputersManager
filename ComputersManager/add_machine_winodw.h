#pragma once

#include "sql_service.h"
#include "ui_add_machine_winodw.h"
#include <qfile.h>
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QWidget>
#include <qmessagebox.h>

/// @brief 机房添加电脑页面
class AddMachineWinodw : public QWidget
{
	Q_OBJECT

public:
	AddMachineWinodw(std::string room_name, QWidget *parent = nullptr);
	~AddMachineWinodw();

signals:
	void signal_modify_finish();		// 信号，提示关闭这个页面

private:
	void closeEvent(QCloseEvent* e);		// 重写关闭事件
	void on_click_pushbutton_cancal();		// 取消按钮槽函数
	void on_click_pushbutton_confirm();		// 确认按钮槽函数
	
private:
	Ui::AddMachineWinodwClass ui;
	std::string room_name_;
};
