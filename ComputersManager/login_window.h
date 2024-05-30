#pragma once

#include <QWidget>
#include "ui_login_window.h"

class LoginWindow : public QWidget
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = nullptr);
	~LoginWindow();

private:
	Ui::LoginWindowClass ui;
};
