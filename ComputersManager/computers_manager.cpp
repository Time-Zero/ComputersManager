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
    
    connect(ui.toolButton_mainpage, &QToolButton::clicked, this, &ComputersManager::on_click_toolbutton_mainpage);
    connect(ui.pushButton_exit_login, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_exit_login);
    connect(ui.pushButton_modify_info, &QPushButton::clicked, this, &ComputersManager::on_click_pushbutton_modify_info);

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

void ComputersManager::slot_login(std::string userid)
{
    delete p_login_window_;
    user_info_.id = userid;     //设置用户
    InitMainPage();
    this->show();
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
}

void ComputersManager::on_click_toolbutton_mainpage()
{
    ui.stackedWidget->setCurrentIndex(MAIN_PAGE);
    InitMainPage();
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
