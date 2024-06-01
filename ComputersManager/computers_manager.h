#pragma once

#include "login_window.h"
#include "ui_computers_manager.h"
#include <qfile.h>
#include <QtWidgets/QWidget>
#include "modify_window.h"


#define MAIN_PAGE 0

class ComputersManager : public QWidget
{
    Q_OBJECT

public:
    ComputersManager(QWidget *parent = nullptr);
    ~ComputersManager();

private:
    void InitMainPage();
    void slot_login(std::string userid);
    void on_click_toolbutton_mainpage();
    void on_click_pushbutton_exit_login();
    void on_click_pushbutton_modify_info();

private:
    Ui::ComputersManagerClass ui;
    LoginWindow* p_login_window_;
    ModifyWindow* p_modify_window_;
    UserInfo user_info_;
};
