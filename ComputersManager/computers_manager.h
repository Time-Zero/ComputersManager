#pragma once

#include "add_machine_winodw.h"
#include "change_userinfo.h"
#include "create_room_window.h"
#include "login_window.h"
#include "modify_machine_window.h"
#include "modify_room.h"
#include "modify_window.h"
#include "ui_computers_manager.h"
#include <qabstractitemview.h>
#include <qfile.h>
#include <qstandarditemmodel.h>
#include <QtWidgets/QWidget>
#include "rent_machine_window.h"
#include "server_connector.h"
#include "qfiledialog.h"
#include "qstandardpaths.h"
#include "configure_get.h"


#define MAIN_PAGE 0
#define ROOM_PAGE 1
#define USER_CONTROL_PAGE 2
#define MACHINE_ROOM_PAGE 3
#define SUM_PAGE 4
#define BACKUP_PAGE 5

#define TABLE_WIDTH 736

/// @brief qtableview中用户表的索引
enum TABLE_USER
{
    TABLE_ID = 0,
    TABLE_NAME = 1,
    TABLE_PERMISSION = 2,
    TABLE_ORDER = 3
};

/// @brief 房间表索引
enum TABLE_ROOM {
    TABLE_ROOM_NAME,
    TABLE_ROOM_STATUES,
    TABLE_ROOM_FEES,
    TABLE_ROOM_MANAGER_ID,
    TABLE_ROOM_MANAGER_NAME
};

/// @brief 机器表索引
enum MACHINE_TABLE {
    MACHINE_ID,
    MACHINE_STATUS,
    MACHINE_CPU,
    MACHINE_RAM,
    MACHINE_ROM,
    MACHINE_GPU,
    MACHINE_UID,
    MACHINE_NAME
};

/// @brief 主窗口
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
    void MachinesTableInit();
    void SumPageInit();
    void BackupPageInit();
    void RefreshMachineTable(QStandardItemModel* model, std::string& room_name);
    void slot_login(std::string userid);
    void on_click_toolbutton_mainpage();
    void on_click_toolbutton_user();
    void on_click_toolbutton_room();
    void on_click_toolbutton_sum();
    void on_click_toolbutton_backup();
    void on_click_toolbutton_select_backup_location();
    void on_click_pushbutton_exit_login();
    void on_click_pushbutton_modify_info();
    void on_click_pushbutton_search_user();
    void on_click_pushbutton_create_room();
    void on_click_pushbutton_room_manager();
    void on_click_pushbutton_delete_room();
    void on_click_pushbutton_machine_manager();
    void on_click_pushbutton_machine_add();
    void on_click_pushbutton_machine_room_sum();
    void on_click_pushbutton_machine_delete();
    void on_click_pushbutton_sum_confirm();
    void on_click_pushbutton_backup_confirm();
    void on_click_tableview_user(const QModelIndex& index);
    void on_click_tableview_room(const QModelIndex& index);
    void on_click_tableview_machine(const QModelIndex& index);

private:
    Ui::ComputersManagerClass ui;
    LoginWindow* p_login_window_;
    ModifyWindow* p_modify_window_;
    UserInfo user_info_;
    std::string current_machine_room;
};
