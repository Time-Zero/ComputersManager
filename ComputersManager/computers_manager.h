#pragma once

#include "login_window.h"
#include "modify_window.h"
#include "ui_computers_manager.h"
#include <qfile.h>
#include <qstandarditemmodel.h>
#include <QtWidgets/QWidget>
#include <qabstractitemview.h>
#include "change_userinfo.h"
#include "create_room_window.h"
#include "modify_room.h"


#define MAIN_PAGE 0
#define ROOM_PAGE 1
#define USER_CONTROL_PAGE 2

#define TABLE_WIDTH 736

enum TABLE_USER
{
    TABLE_ID = 0,
    TABLE_NAME = 1,
    TABLE_PERMISSION = 2,
    TABLE_ORDER = 3
};

enum TABLE_ROOM {
    TABLE_ROOM_NAME = 0,
    TABLE_ROOM_STATUES = 1,
    TABLE_ROOM_MANAGER_ID = 2,
    TABLE_ROOM_MANAGER_NAME = 3
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
    void RoomTableInit();
    void slot_login(std::string userid);
    void on_click_toolbutton_mainpage();
    void on_click_toolbutton_user();
    void on_click_toolbutton_room();
    void on_click_pushbutton_exit_login();
    void on_click_pushbutton_modify_info();
    void on_click_pushbutton_search_user();
    void on_click_pushbutton_create_room();
    void on_click_pushbutton_room_manager();
    void on_click_tableview_user(const QModelIndex& index);

private:
    Ui::ComputersManagerClass ui;
    LoginWindow* p_login_window_;
    ModifyWindow* p_modify_window_;
    UserInfo user_info_;
};
