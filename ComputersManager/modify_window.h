#pragma once

#include "sql_service.h"
#include "ui_modify_window.h"
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QWidget>
#include <qmessagebox.h>
#include "common_tools.h"
#include <future>
#include <qfile.h>

class ModifyWindow : public QWidget
{
	Q_OBJECT

public:
	ModifyWindow(QWidget *parent = nullptr, std::string userid = "");
	~ModifyWindow();

signals:
	void signal_modify_finish();

private:
	void on_click_button_cancal();
	void on_click_button_confirm();

private:
	Ui::ModifyWindowClass ui;
	std::string userid_;
};
