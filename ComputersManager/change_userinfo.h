#pragma once

#include "sql_service.h"
#include "ui_change_userinfo.h"
#include <future>
#include <qfile.h>
#include <qmessagebox.h>
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QWidget>

/// @brief 修改用户信息窗口
class ChangeUserInfo : public QWidget
{
	Q_OBJECT

public:
	ChangeUserInfo(std::string& userid, unsigned int permission,QWidget *parent = nullptr);
	~ChangeUserInfo();

signals:
	void signal_change_finish();		// 信号，用于让主窗口关闭这个窗口

private:
	void on_click_button_confirm();
	void closeEvent(QCloseEvent* e);

private:
	Ui::ChangeUserInfoClass ui;
};
