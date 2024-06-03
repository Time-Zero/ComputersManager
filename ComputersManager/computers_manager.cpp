#include "computers_manager.h"

ComputersManager::ComputersManager(QWidget *parent)
    : QWidget(parent),
    p_modify_window_(nullptr)
{
    ui.setupUi(this);
    QFile file(":/ComputersManager/computer_manager.qss");
    if (file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
        file.close();
    }
    
    UserTableInit();
    RoomTableInit();

    connect(ui.toolButton_mainpage, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_mainpage);
    connect(ui.toolButton_user, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_user);
    connect(ui.toolButton_room, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_room);
    connect(ui.pushButton_exit_login, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_exit_login);
    connect(ui.pushButton_modify_info, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_modify_info);
    connect(ui.pushButton_search_user, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_search_user);
    connect(ui.pushButton_create_room, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_create_room);
    p_login_window_ = new LoginWindow();
    connect(p_login_window_, &LoginWindow::signal_login, this, &ComputersManager::slot_login);
    p_login_window_->show();
}

ComputersManager::~ComputersManager()
{}

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

void ComputersManager::UserTableInit()
{
    ui.lineEdit_search_user->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    ui.lineEdit_search_user->setPlaceholderText(QString::fromStdString("输入学号以搜索"));
    QStandardItemModel* user_table_model = new QStandardItemModel(this);
    QList<QString> user_table_header{"学号","姓名"};
    user_table_model->setHorizontalHeaderLabels(QStringList(user_table_header));
    ui.tableView_user->setModel(user_table_model);
    ui.tableView_user->setFocusPolicy(Qt::NoFocus);
    ui.tableView_user->setFrameShape(QFrame::NoFrame);
    ui.tableView_user->setAlternatingRowColors(true);
    ui.tableView_user->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int i = 0; i < user_table_header.size(); i++) {
        ui.tableView_user->setColumnWidth(i, 368);
    }
    connect(ui.tableView_user, &QAbstractItemView::doubleClicked, this, &ComputersManager::on_click_tableview_user);
}

void ComputersManager::RoomTableInit()
{
    QStandardItemModel* room_table_model = new QStandardItemModel(this);
    QList<QString> room_table_header{ "机房名","状态","管理员ID","管理员姓名" };
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

    //TODO:删除机房和进入机房
}

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
        ui.toolButton_sum->setVisible(true);
        ui.toolButton_order->setVisible(true);
        ui.toolButton_backup->setVisible(true);
    }

    if (user_info_.permission != SADMIN) {
        ui.pushButton_create_room->setEnabled(false);
    }

    this->show();
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
}

void ComputersManager::on_click_toolbutton_mainpage()
{
    if (ui.stackedWidget->currentIndex() == MAIN_PAGE)
        return;
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
    InitMainPage();
}

void ComputersManager::on_click_toolbutton_user()
{
    if (ui.stackedWidget->currentIndex() == USER_CONTROL_PAGE)
        return;

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
        
    }
}

void ComputersManager::on_click_toolbutton_room()
{
    if (ui.stackedWidget->currentIndex() == ROOM_PAGE)
        return;

    ui.stackedWidget->setCurrentIndex(ROOM_PAGE);
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
        delete create_room_window;
        });
    create_room_window->show();
}

void ComputersManager::on_click_tableview_user(const QModelIndex& index)
{
    int row = index.row();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.tableView_user->model());
    std::string user_id = model->index(row, 0, QModelIndex()).data().toString().toStdString();
    
    ChangeUserInfo* change_window = new ChangeUserInfo(user_id, user_info_.permission, nullptr);
    connect(change_window, &ChangeUserInfo::signal_change_finish, this, [=]() {
        delete change_window;
        });
    change_window->show();
}
