#include "computers_manager.h"

ComputersManager::ComputersManager(QWidget *parent)
    : QWidget(parent),
    p_modify_window_(nullptr),
    current_machine_room("")
{
    ui.setupUi(this);
    QFile file(":/ComputersManager/computer_manager.qss");
    if (file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
        file.close();
    }
    
    UserTableInit();
    RoomTableInit();
    MachinesTableInit();
    SumPageInit();

    connect(ui.toolButton_mainpage, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_mainpage);
    connect(ui.toolButton_user, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_user);
    connect(ui.toolButton_room, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_room);
    connect(ui.pushButton_exit_login, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_exit_login);
    connect(ui.pushButton_modify_info, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_modify_info);
    connect(ui.pushButton_search_user, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_search_user);
    connect(ui.pushButton_create_room, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_create_room);
    connect(ui.pushButton_room_manager, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_room_manager);
    connect(ui.pushButton_delete_room, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_delete_room);
    connect(ui.pushButton_machine_delete, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_machine_delete);
    connect(ui.pushButton_machine_manager, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_machine_manager);
    connect(ui.pushButton_machine_add, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_machine_add);
    connect(ui.pushButton_room_sum, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_machine_room_sum);
    connect(ui.toolButton_sum, &QPushButton::clicked, this, &ComputersManager::on_click_toolbutton_sum);
    connect(ui.pushButton_sum_confirm, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_sum_confirm);
    
    p_login_window_ = new LoginWindow();
    connect(p_login_window_, &LoginWindow::signal_login, this, &ComputersManager::slot_login);
    p_login_window_->show();
}

ComputersManager::~ComputersManager()
{}


/// @brief 初始化主页
void ComputersManager::InitMainPage()
{
    std::future<void> fut = std::async(std::launch::async, [&]() {
        SqlService::GetInstance().GetUserInfo(user_info_);
    });

    while (fut.wait_for(std::chrono::microseconds(SQL_TIMEOUT)) != std::future_status::ready) {
        QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("与后台断开连接"));
        return;
    }

    fut.get();
    
    ui.label_mainpage_userid->setText(QString::fromStdString(user_info_.id));
    ui.label_mainpage_username->setText(QString::fromStdString(user_info_.name));
    ui.label_mainpage_permission->setText(QString::fromStdString(TransPermissionCodeToString(user_info_.permission)));
    if (user_info_.order) {
        ui.label_mainpage_order->setText(QString::fromStdString("允许预约"));
    }
    else {
        ui.label_mainpage_order->setText(QString::fromStdString("禁止预约"));
    }
}

/// @brief 初始化用户表
void ComputersManager::UserTableInit()
{
    ui.lineEdit_search_user->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui.lineEdit_search_user->setPlaceholderText(QString::fromStdString("输入学号以搜索"));
    QStandardItemModel* user_table_model = new QStandardItemModel(this);
    QList<QString> user_table_header{"学号","姓名","权限","允许预约"};
    user_table_model->setHorizontalHeaderLabels(QStringList(user_table_header));
    ui.tableView_user->setModel(user_table_model);
    ui.tableView_user->setFocusPolicy(Qt::NoFocus);
    ui.tableView_user->setFrameShape(QFrame::NoFrame);
    ui.tableView_user->setAlternatingRowColors(true);
    ui.tableView_user->setSelectionBehavior(QAbstractItemView::SelectRows);
    int header_size = user_table_header.size();
    int col_width = TABLE_WIDTH / header_size;
    for (int i = 0; i < header_size; i++) {
        ui.tableView_user->setColumnWidth(i, col_width);
    }
    connect(ui.tableView_user, &QAbstractItemView::doubleClicked, this, &ComputersManager::on_click_tableview_user);
}

/// @brief 初始化机房表
void ComputersManager::RoomTableInit()
{
    QStandardItemModel* room_table_model = new QStandardItemModel(this);
    QList<QString> room_table_header{ "机房名","状态","计费(元/小时)","管理员ID","管理员姓名"};
    room_table_model->setHorizontalHeaderLabels(QStringList(room_table_header));
    ui.tableView_rooms->setModel(room_table_model);
    ui.tableView_rooms->setFocusPolicy(Qt::NoFocus);
    ui.tableView_rooms->setFrameShape(QFrame::NoFrame);
    ui.tableView_rooms->setAlternatingRowColors(true);
    ui.tableView_rooms->setSelectionBehavior(QAbstractItemView::SelectRows);
    int header_size = room_table_header.size();
    int col_width = TABLE_WIDTH / header_size;
    for (int i = 0; i < header_size; i++) {
        ui.tableView_rooms->setColumnWidth(i, col_width);
    }
    connect(ui.tableView_rooms, &QTableView::doubleClicked, this, &ComputersManager::on_click_tableview_room);
}


/// @brief 初始化机器表
void ComputersManager::MachinesTableInit()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    QList<QString> table_header{ "编号","状态","CPU","Ram","Rom","Gpu","使用者ID","姓名"};
    model->setHorizontalHeaderLabels(QStringList(table_header));
    ui.tableView_machines->verticalHeader()->setVisible(false);
    ui.tableView_machines->setModel(model);         
    ui.tableView_machines->setFocusPolicy(Qt::NoFocus);             //选中没有虚线框
    ui.tableView_machines->setFrameShape(QFrame::NoFrame);          // 没有边框
    ui.tableView_machines->setAlternatingRowColors(true);           // 交替颜色
    ui.tableView_machines->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中行为为选中行
    int header_size = table_header.size();
    int col_width = TABLE_WIDTH / header_size;
    for (int i = 0; i < header_size; i++) {
        ui.tableView_machines->setColumnWidth(i, col_width);
    }

    connect(ui.tableView_machines, &QTableView::doubleClicked, this, &ComputersManager::on_click_tableview_machine);
}

void ComputersManager::SumPageInit()
{
    ui.dateTimeEdit_end_time->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui.dateTimeEdit_end_time->setCalendarPopup(true);
    ui.dateTimeEdit_end_time->setDateTime(QDateTime::currentDateTime());
    ui.dateTimeEdit_start_time->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui.dateTimeEdit_start_time->setCalendarPopup(true);

    QStandardItemModel* model_people = new QStandardItemModel();
    QList<QString> table_header_people{ "工号","姓名" };
    model_people->setHorizontalHeaderLabels(QStringList(table_header_people));
    ui.tableView_sum_people->setModel(model_people);
    ui.tableView_sum_people->setFocusPolicy(Qt::NoFocus);             //选中没有虚线框
    ui.tableView_sum_people->setFrameShape(QFrame::NoFrame);          // 没有边框
    ui.tableView_sum_people->setAlternatingRowColors(true);           // 交替颜色
    ui.tableView_sum_people->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中行为为选中行
    for (int i = 0; i < table_header_people.size(); i++) {
        ui.tableView_sum_people->setColumnWidth(i, 367/2);
    }

    QStandardItemModel* model_room = new QStandardItemModel();
    QList<QString> table_header_room{ "机房名" };
    model_room->setHorizontalHeaderLabels(QStringList(table_header_room));
    ui.tableView_sum_room->setModel(model_room);
    ui.tableView_sum_room->setFocusPolicy(Qt::NoFocus);             //选中没有虚线框
    ui.tableView_sum_room->setFrameShape(QFrame::NoFrame);          // 没有边框
    ui.tableView_sum_room->setAlternatingRowColors(true);           // 交替颜色
    ui.tableView_sum_room->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中行为为选中行
    ui.tableView_sum_room->setColumnWidth(0, 367);
}

/// @brief 刷新机器表
/// @param model 表的模式
/// @param room_name 机房名字
void ComputersManager::RefreshMachineTable(QStandardItemModel* model, std::string& room_name)
{
    TableClear(model);
    auto machines = SqlService::GetInstance().GetMachines(room_name);
    while (!machines.empty()) {
        int row = model->rowCount();
        Machine machine = machines.front();
        machines.pop();

        QStandardItem* id = new QStandardItem(QString::fromStdString(std::to_string(machine.id)));
        id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_ID, id);

        std::string str_status;
        if (machine.status == 0) str_status = "停用";
        else if (machine.status == 1) str_status = "空闲";
        else str_status = "使用";
        QStandardItem* status = new QStandardItem(QString::fromStdString(str_status));
        status->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_STATUS, status);

        QStandardItem* cpu = new QStandardItem(QString::fromStdString(machine.cpu));
        cpu->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_CPU, cpu);

        QStandardItem* ram = new QStandardItem(QString::fromStdString(machine.ram));
        ram->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_RAM, ram);

        QStandardItem* rom = new QStandardItem(QString::fromStdString(machine.rom));
        rom->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_ROM, rom);

        QStandardItem* gpu = new QStandardItem(QString::fromStdString(machine.gpu));
        gpu->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_GPU, gpu);

        QStandardItem* uid = new QStandardItem(QString::fromStdString(machine.uid));
        uid->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_UID, uid);

        QStandardItem* uname = new QStandardItem(QString::fromStdString(machine.uname));
        uname->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, MACHINE_NAME, uname);
    }
}

/// @brief 登录槽函数
/// @param userid 用户的id
void ComputersManager::slot_login(std::string userid)
{
    delete p_login_window_;
    user_info_.id = userid;     //设置用户
    InitMainPage();

    if (user_info_.permission == USER) {
        ui.toolButton_mainpage->setVisible(true);
        ui.toolButton_user->setVisible(false);
        ui.toolButton_room->setVisible(true);
        ui.toolButton_sum->setVisible(false);
        ui.toolButton_order->setVisible(false);
        ui.toolButton_backup->setVisible(false);
    }
    else {
        ui.toolButton_mainpage->setVisible(true);
        ui.toolButton_user->setVisible(true);
        ui.toolButton_room->setVisible(true);
        if (user_info_.permission != SADMIN) ui.toolButton_sum->setVisible(false);
        else ui.toolButton_sum->setVisible(true);
        ui.toolButton_order->setVisible(false);
        ui.toolButton_backup->setVisible(true);
    }

    // 只有超级管理员允许创建机房和删除机房
    if (user_info_.permission != SADMIN) {
        ui.pushButton_create_room->setEnabled(false);
        ui.pushButton_delete_room->setEnabled(false);       
        ui.pushButton_room_manager->setEnabled(false);
    }
    else {
        ui.pushButton_create_room->setEnabled(true);
        ui.pushButton_delete_room->setEnabled(true);
        ui.pushButton_room_manager->setEnabled(true);
    }

    //只有管理员有权限删除和管理机器
    if (user_info_.permission < ADMIN) {
        ui.pushButton_machine_manager->setEnabled(false);
        ui.pushButton_machine_delete->setEnabled(false);
        ui.pushButton_machine_add->setEnabled(false);
    }
    else {
        ui.pushButton_machine_manager->setEnabled(true);
        ui.pushButton_machine_delete->setEnabled(true);
        ui.pushButton_machine_add->setEnabled(true);
    }

    this->show();
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
}


/// @brief 主页键点击的槽函数
void ComputersManager::on_click_toolbutton_mainpage()
{
    if (ui.stackedWidget->currentIndex() == MAIN_PAGE)
        return;
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
    InitMainPage();
}


/// @brief 用户键点击的槽函数
void ComputersManager::on_click_toolbutton_user()
{
    /*if (ui.stackedWidget->currentIndex() == USER_CONTROL_PAGE)
        return;*/

    std::future<std::queue<UserInfo>> fut_ret = std::async(std::launch::async, [&]() {
        return SqlService::GetInstance().GetUserList(user_info_);
        });
    
    ui.lineEdit_search_user->clear();
    ui.stackedWidget->setCurrentIndex(USER_CONTROL_PAGE);

    QStandardItemModel* model = (QStandardItemModel*)ui.tableView_user->model();
    TableClear(model);

    std::queue<UserInfo> user_list = fut_ret.get();
    while (!user_list.empty()) {
        UserInfo user = user_list.front();
        user_list.pop();
        int row = model->rowCount();
        
        QStandardItem* item_id = new QStandardItem(QString::fromStdString(user.id));
        item_id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ID, item_id);

        QStandardItem* item_name = new QStandardItem(QString::fromStdString(user.name));
        item_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_NAME, item_name);
        
        std::string str_permission = TransPermissionCodeToString(user.permission);
        QStandardItem* item_permission = new QStandardItem(QString::fromStdString(str_permission));
        item_permission->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_PERMISSION, item_permission);
        
        std::string str_order = user.order ? "是" : "否";
        QStandardItem* item_order = new QStandardItem(QString::fromStdString(str_order));
        item_order->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ORDER, item_order);
    }
}

void ComputersManager::on_click_toolbutton_room()
{

    auto fut_ret = std::async(std::launch::async, [=]() {
        return SqlService::GetInstance().GetRoomList();
        });
    
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_rooms->model());
    TableClear(model);

    std::vector<std::vector<std::string>> ret = fut_ret.get();
    for (std::vector<std::string> it : ret) {
        int row = model->rowCount();

        QStandardItem* item_room_name = new QStandardItem(QString::fromStdString(it[TABLE_ROOM_NAME]));
        item_room_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ROOM_NAME, item_room_name);

        std::string str_status = (it[TABLE_ROOM_STATUES] == "1") ? "启用" : "停用";
        QStandardItem* item_room_status = new QStandardItem(QString::fromStdString(str_status));
        item_room_status->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ROOM_STATUES, item_room_status);

        QStandardItem* item_room_fess = new QStandardItem(QString::fromStdString(it[TABLE_ROOM_FEES]));
        item_room_status->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ROOM_FEES, item_room_fess);

        QStandardItem* item_room_manager_id = new QStandardItem(QString::fromStdString(it[TABLE_ROOM_MANAGER_ID]));
        item_room_manager_id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ROOM_MANAGER_ID, item_room_manager_id);

        QStandardItem* item_room_manager_name = new QStandardItem(QString::fromStdString(it[TABLE_ROOM_MANAGER_NAME]));
        item_room_manager_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(row, TABLE_ROOM_MANAGER_NAME, item_room_manager_name);
    }


    if (ui.stackedWidget->currentIndex() == ROOM_PAGE)
        return;
    ui.stackedWidget->setCurrentIndex(ROOM_PAGE);
    
}

void ComputersManager::on_click_toolbutton_sum()
{
    ui.stackedWidget->setCurrentIndex(SUM_PAGE);
    auto manager_list = SqlService::GetInstance().GetManagerList();
    auto machine_room_list = SqlService::GetInstance().GetSimpleRoomList();

    {
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_sum_people->model());
        TableClear(model);
        for (auto manager : manager_list) {
            int row = model->rowCount();

            QStandardItem* item1 = new QStandardItem(QString::fromStdString(manager.first));
            item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            model->setItem(row, 0, item1);

            QStandardItem* item2 = new QStandardItem(QString::fromStdString(manager.second));
            item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            model->setItem(row, 1, item2);
        }
    }

    {
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_sum_room->model());
        TableClear(model);
        for (auto room : machine_room_list) {
            int row = model->rowCount();

            QStandardItem* item = new QStandardItem(QString::fromStdString(room));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            model->setItem(row, 0, item);
        }
    }

}

void ComputersManager::on_click_pushbutton_exit_login()
{
    user_info_.id = "";
    user_info_.name = "";
    user_info_.password = "";
    user_info_.order = 0;
    user_info_.permission = 0;

    this->setVisible(false);
    p_login_window_ = new LoginWindow(); 
    connect(p_login_window_, &LoginWindow::signal_login, this, &ComputersManager::slot_login);
    p_login_window_->show();
}

void ComputersManager::on_click_pushbutton_modify_info()
{
    p_modify_window_ = new ModifyWindow(nullptr, user_info_.id);
    connect(p_modify_window_, &ModifyWindow::signal_modify_finish, [this]()->void {delete p_modify_window_; });
    p_modify_window_->show();
}

void ComputersManager::on_click_pushbutton_search_user()
{
    std::string user_id = ui.lineEdit_search_user->text().toStdString();
    if (user_id.empty()) {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号不能为空"), QMessageBox::Ok);
        return;
    }
    else if(user_id.size() != 10){
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("学号非法"), QMessageBox::Ok);
        return;
    }

    std::future<std::string> fut_name = std::async(std::launch::async, [&]() {
        return SqlService::GetInstance().GetUserName(user_id, user_info_.permission);
        });

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_user->model());
    TableClear(model);
    
    while (fut_name.wait_for(std::chrono::microseconds(SQL_TIMEOUT)) != std::future_status::ready) {
        QMessageBox::warning(this, QStringLiteral("严重错误"), QStringLiteral("与后端断开连接"), QMessageBox::Ok);
        return;
    }
    std::string user_name = fut_name.get();
    if (!user_name.empty()) {
        QStandardItem* item_id = new QStandardItem(QString::fromStdString(user_id));
        item_id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(0, TABLE_ID, item_id);

        QStandardItem* item_name = new QStandardItem(QString::fromStdString(user_name));
        item_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        model->setItem(0, TABLE_NAME, item_name);
    }
}

void ComputersManager::on_click_pushbutton_create_room()
{
    CreateRoomWindow* create_room_window = new CreateRoomWindow();
    connect(create_room_window, &CreateRoomWindow::signal_create_room_finish, this, [=]() {
        on_click_toolbutton_room();
        delete create_room_window;
        });
    create_room_window->show();
}

void ComputersManager::on_click_pushbutton_room_manager()
{
    QModelIndex ret = ui.tableView_rooms->currentIndex();
    int row = ret.row();
    if (row == -1) {
        QMessageBox::information(this, QStringLiteral("注意"), QStringLiteral("请选择要修改的机房"));
        return;
    }

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_rooms->model());
    std::string room_name = model->index(row, TABLE_ROOM_NAME, QModelIndex()).data().toString().toStdString();
    std::vector<std::string> ret_room_info =  SqlService::GetInstance().GetRoomInfo(room_name);

    std::string room_status = (ret_room_info[0] == "1") ? "启用" : "停用";
    QStandardItem* item_room_name = new QStandardItem(QString::fromStdString(room_status));
    item_room_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    model->setItem(row, TABLE_ROOM_STATUES, item_room_name);

    QStandardItem* item_room_manager_id = new QStandardItem(QString::fromStdString(ret_room_info[1]));
    item_room_manager_id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    model->setItem(row, TABLE_ROOM_MANAGER_ID, item_room_manager_id);
    if (ret_room_info[1] != user_info_.id && user_info_.permission < SADMIN) {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("你不是超级管理员或者机房的管理员"));
        return;
    }

    ModifyRoom* modify_room_window = new ModifyRoom(room_name, nullptr);
    connect(modify_room_window, &ModifyRoom::signal_modify_finish, this, [=]() {
        on_click_toolbutton_room();
        delete modify_room_window;
        });
    modify_room_window->show();
}

void ComputersManager::on_click_pushbutton_delete_room()
{
    QModelIndex index = ui.tableView_rooms->currentIndex();
    int row = index.row();
    if (row == -1) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要操作的机房"));
        return;
    }

    auto ret_button = QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("确认删除机房?"), QMessageBox::Ok | QMessageBox::Cancel);
    if (ret_button == QMessageBox::Cancel)
        return;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_rooms->model());
    std::string room_name = model->index(row, TABLE_ROOM_NAME, QModelIndex()).data().toString().toStdString();
    unsigned int ret = SqlService::GetInstance().DeleteMachineRoom(room_name);
    if (ret == 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("删除成功"),QMessageBox::Ok);
    }
    else if(ret == 1){
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("无法删除，还有人在使用机房"), QMessageBox::Ok);
        return;
    }
    else {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("发生了未知错误"), QMessageBox::Ok);
        return;
    }

    on_click_toolbutton_room();
}

void ComputersManager::on_click_pushbutton_machine_manager()
{
    int row = ui.tableView_machines->currentIndex().row();
    if (row == -1) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要操作的设备"));
        return;
    }

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_machines->model());
    std::string machine_id = model->index(row, MACHINE_ID, QModelIndex()).data().toString().toStdString();

    ModifyMachineWindow* modify_machine_window = new ModifyMachineWindow(current_machine_room, machine_id);
    connect(modify_machine_window, &ModifyMachineWindow::signal_modify_finish, this, [=]() {
        delete modify_machine_window;
        RefreshMachineTable(model, current_machine_room);
        });
    modify_machine_window->show();
}

void ComputersManager::on_click_pushbutton_machine_add()
{
    AddMachineWinodw* add_machine_window = new AddMachineWinodw(current_machine_room);
    connect(add_machine_window, &AddMachineWinodw::signal_modify_finish, this, [=]() {
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_machines->model());
        RefreshMachineTable(model, current_machine_room);           // 刷新表
        delete add_machine_window;          //删除添加窗口
        });

    add_machine_window->show();
}

void ComputersManager::on_click_pushbutton_machine_room_sum()
{
    int row = ui.tableView_rooms->currentIndex().row();
    if (row == -1) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要操作的设备"));
        return;
    }

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_rooms->model());
    std::string room_name = model->index(row, TABLE_ROOM_NAME, QModelIndex()).data().toString().toStdString();

    std::vector<std::string> ret = SqlService::GetInstance().GetRoomSum(room_name);
    std::string sum_message = "上机时间总计：" + ret[0] + "小时 \r\n 上机费用总计： " + ret[1] + "元";
    QMessageBox::information(this, QStringLiteral("提示"), QString::fromStdString(sum_message));
}

void ComputersManager::on_click_pushbutton_machine_delete()
{
    int row = ui.tableView_machines->currentIndex().row();
    if (row == -1) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要操作的设备"));
        return;
    }

    auto button_ret = QMessageBox::question(this, QStringLiteral("提示"), QStringLiteral("确认要删除吗"), QMessageBox::Ok | QMessageBox::Cancel);
    if (button_ret != QMessageBox::Ok)
        return;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_machines->model());
    std::string machine_id = model->index(row, MACHINE_ID, QModelIndex()).data().toString().toStdString();
    
    unsigned int ret = SqlService::GetInstance().DeleteMachine(machine_id, current_machine_room);
    if (ret == 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("删除成功"));
        RefreshMachineTable(model, current_machine_room);           //刷新一下表的显示
    }
    else {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("删除失败，未知错误"));
    }
}

void ComputersManager::on_click_pushbutton_sum_confirm()
{
    std::string manager_id;
    std::string room_name;
    std::string start_time = ui.dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString();
    std::string end_time = ui.dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString();

    {
        int row = ui.tableView_sum_people->currentIndex().row();
        if (row == -1) {
            manager_id = "";
        }
        else {
            QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_sum_people->model());
            manager_id = model->index(row, 0, QModelIndex()).data().toString().toStdString();
        }
    }

    {
        int row = ui.tableView_sum_room->currentIndex().row();
        if (row == -1) {
            room_name = "";
        }
        else {
            QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_sum_room->model());
            room_name = model->index(row, 0, QModelIndex()).data().toString().toStdString();
        }
    }

    auto ret = SqlService::GetInstance().CountItBySomeRows(manager_id, room_name, start_time, end_time);
    std::string mess = "统计结果为: " + std::to_string(ret) + "元";

    QMessageBox::information(this, QStringLiteral("提示"), QString::fromStdString(mess));
}

void ComputersManager::on_click_tableview_user(const QModelIndex& index)
{
    int row = index.row();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_user->model());
    std::string user_id = model->index(row, TABLE_ID, QModelIndex()).data().toString().toStdString();
    
    ChangeUserInfo* change_window = new ChangeUserInfo(user_id, user_info_.permission, nullptr);
    connect(change_window, &ChangeUserInfo::signal_change_finish, this, [=]() {
        on_click_toolbutton_user();
        delete change_window;
        });
    change_window->show();
}

void ComputersManager::on_click_tableview_room(const QModelIndex& index)
{
    int row = index.row();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_rooms->model());
    std::string room_name = model->index(row, TABLE_ROOM_NAME, QModelIndex()).data().toString().toStdString();
    std::string room_status = model->index(row, TABLE_ROOM_STATUES, QModelIndex()).data().toString().toStdString();
    if (room_status != "启用") {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("该机房已被停用"));
        return;
    }


    current_machine_room = room_name;                   //记录一下现在打开的机房，用于后面的机房电脑表的定位
    model = static_cast<QStandardItemModel*>(ui.tableView_machines->model());
    RefreshMachineTable(model, room_name);

    std::string manager_id = SqlService::GetInstance().GetRoomManager(current_machine_room);
    if (manager_id == user_info_.id || user_info_.permission == SADMIN) {
        ui.pushButton_machine_add->setEnabled(true);
        ui.pushButton_machine_manager->setEnabled(true);
        ui.pushButton_machine_delete->setEnabled(true);
    }
    else {
        ui.pushButton_machine_add->setEnabled(false);
        ui.pushButton_machine_manager->setEnabled(false);
        ui.pushButton_machine_delete->setEnabled(false);
    }
    
    ui.stackedWidget->setCurrentIndex(MACHINE_ROOM_PAGE); 
}


void ComputersManager::on_click_tableview_machine(const QModelIndex& index)
{
    //用户不允许操作
    if (user_info_.permission == USER)
        return;

    // 非本机房的管理员或者不是超级管理员的不允许操作上机
    std::string manager_id = SqlService::GetInstance().GetRoomManager(current_machine_room);
    if (manager_id != user_info_.id || user_info_.permission < SADMIN)
        return;

    int row = index.row();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_machines->model());
    std::string machine_id = model->index(row, MACHINE_ID, QModelIndex()).data().toString().toStdString();
    std::string machine_user_id = model->index(row, MACHINE_UID, QModelIndex()).data().toString().toStdString();
        

    RentMachineWindow* rent_machine_window = new RentMachineWindow(current_machine_room ,machine_id);
    connect(rent_machine_window, &RentMachineWindow::signal_rent_finish, this, [=]() {
        RefreshMachineTable(model, current_machine_room);
        delete rent_machine_window;
        });
    rent_machine_window->show();
}
