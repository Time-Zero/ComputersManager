#include "rent_machine_window.h"

RentMachineWindow::RentMachineWindow(std::string room_name, std::string machine_id, QWidget *parent)
	: QWidget(parent),
	room_name_(room_name),
	machine_id_(machine_id)
{
	ui.setupUi(this);
	
}

RentMachineWindow::~RentMachineWindow()
{}


void RentMachineWindow::closeEvent(QCloseEvent * e)
{
	emit signal_rent_finish();
}
