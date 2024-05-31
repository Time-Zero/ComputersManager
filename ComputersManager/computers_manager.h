#pragma once

#include <QtWidgets/QWidget>
#include "ui_computers_manager.h"
#include "login_window.h"

class ComputersManager : public QWidget
{
    Q_OBJECT

public:
    ComputersManager(QWidget *parent = nullptr);
    ~ComputersManager();

private:
    void slot_login();

private:
    Ui::ComputersManagerClass ui;
    LoginWindow* p_login_window_;
};
