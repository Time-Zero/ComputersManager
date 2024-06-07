#pragma once

#include <QWidget>
#include "ui_rent_machine_window.h"
#include "sql_service.h"
#include <qfile.h>
#include <future>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qregularexpression.h>

enum StackWidgetIndex {
	PAGE_ON_USE,
	PAGE_NO_USE
};


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
	void on_click_pushbutton_find_user();
	void on_click_pushbutton_rent();


private:
	Ui::RentMachineWindowClass ui;
	std::string room_name_;
	std::string machine_id_;
};
