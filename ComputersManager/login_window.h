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
#include "qstandardpaths.h"
#include "qfiledialog.h"
#include "server_connector.h"
#include "configure_get.h"
#include "memory"

/// @brief 登录页面
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
	void on_click_toolbutton_restore_file_select();
	void on_click_pushbutton_restore_confirm();

private:
	Ui::LoginWindowClass ui;
};
