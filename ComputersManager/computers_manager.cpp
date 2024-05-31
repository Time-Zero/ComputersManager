#include "computers_manager.h"

ComputersManager::ComputersManager(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    p_login_window_ = new LoginWindow();
    connect(p_login_window_, &LoginWindow::signal_login, this, &ComputersManager::slot_login);
    p_login_window_->show();
}

ComputersManager::~ComputersManager()
{}

void ComputersManager::slot_login()
{
    this->show();
    delete p_login_window_;
}
