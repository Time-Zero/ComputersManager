#pragma once

#include <QtWidgets/QWidget>
#include "ui_computers_manager.h"

class ComputersManager : public QWidget
{
    Q_OBJECT

public:
    ComputersManager(QWidget *parent = nullptr);
    ~ComputersManager();

private:
    Ui::ComputersManagerClass ui;
};
