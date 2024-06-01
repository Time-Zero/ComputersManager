#pragma once

#include <QWidget>
#include <qregularexpression.h>
#include <qvalidator.h>
#include "ui_login_window.h"
#include <qfile.h>
#include "common_tools.h"
#include <sql_service.h>
#include <qmessagebox.h>
#include <future>

class LoginWindow : public QWidget
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = nullptr);
	~LoginWindow();

signals:
	void signal_login(std::string userid);

private:
	void on_click_button_login();
	void on_click_button_register();
	void on_click_button_register_confirm();
	void on_click_button_register_cancal();

private:
	Ui::LoginWindowClass ui;
};
