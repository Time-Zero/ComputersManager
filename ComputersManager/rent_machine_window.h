#pragma once

#include <QWidget>
#include "ui_rent_machine_window.h"

class RentMachineWindow : public QWidget
{
	Q_OBJECT

public:
	RentMachineWindow(std::string room_name ,std::string machine_id ,QWidget *parent = nullptr);
	~RentMachineWindow();

signals:
	void signal_rent_finish();

private:
	void closeEvent(QCloseEvent* e);

private:
	Ui::RentMachineWindowClass ui;
	std::string room_name_;
	std::string machine_id_;
};
