#pragma once

#include "sql_service.h"
#include "ui_change_userinfo.h"
#include <future>
#include <qfile.h>
#include <qmessagebox.h>
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QWidget>

class ChangeUserInfo : public QWidget
{
	Q_OBJECT

public:
	ChangeUserInfo(std::string& userid, unsigned int permission,QWidget *parent = nullptr);
	~ChangeUserInfo();

signals:
	void signal_change_finish();

private:
	void on_click_button_confirm();
	void closeEvent(QCloseEvent* e);

private:
	Ui::ChangeUserInfoClass ui;
};
