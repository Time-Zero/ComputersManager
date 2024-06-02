#pragma once

#include "login_window.h"
#include "modify_window.h"
#include "ui_computers_manager.h"
#include <qfile.h>
#include <qstandarditemmodel.h>
#include <QtWidgets/QWidget>
#include <qabstractitemview.h>
#include "change_userinfo.h"


#define MAIN_PAGE 0
#define USER_CONTROL_PAGE 1

enum TABLE_USER
{
    TABLE_ID = 0,
    TABLE_NAME = 1,
};

class ComputersManager : public QWidget
{
    Q_OBJECT

public:
    ComputersManager(QWidget *parent = nullptr);
    ~ComputersManager();

private:
    void InitMainPage();
    void UserTableInit();
    void slot_login(std::string userid);
    void on_click_toolbutton_mainpage();
    void on_click_toolbutton_user();
    void on_click_pushbutton_exit_login();
    void on_click_pushbutton_modify_info();
    void on_click_pushbutton_search_user();
    void on_click_tableview_user(const QModelIndex& index);

private:
    Ui::ComputersManagerClass ui;
    LoginWindow* p_login_window_;
    ModifyWindow* p_modify_window_;
    UserInfo user_info_;
};
