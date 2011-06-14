#include "manualcheckin.h"
#include "api.h"
#include <QtGui>
#include <windows.h>
#include <stdio.h>

ManualCheckin::ManualCheckin(QWidget *parent): QDialog(parent)
{

	ui.setupUi(this);
	ui.eventID->setText(api_get_eventid());
}

ManualCheckin::~ManualCheckin(){

}

void ManualCheckin::on_okButton_clicked(){
	if(ui.ticketID->text().count("-") == 3){
		int result = api_checkin(ui.ticketID->text(), this->bts);
	}else if(!ui.firstName->text().isNull() && !ui.lastName->text().isNull()){
		int result = api_checkin_name(ui.firstName->text(), ui.lastName->text(), ui.eventID->text().toInt(), this->bts);

	}else{
		QMessageBox::information(this->bts, "Error", "Bad ticket ID!");
	}
	

/*	int result = api_login(ui.username->text(), ui.password->text());
	if(HIWORD(result) == STATUS_HTTP_OK && LOWORD(result) == STATUS_LOGIN_OK){
		ui.username->setEnabled(false);
		ui.password->setEnabled(false);
		ui.loginButton->setEnabled(false);
		//ui.logoutButton->setEnabled(true);
		this->done(DialogCode::Accepted);
	}else{
		ui.password->clear();
		this->done(DialogCode::Rejected);
	}
*/
}

void ManualCheckin::on_CancelButton_clicked(){
	/*
	if(!api_logout()){
		ui.username->setEnabled(true);
		ui.password->setEnabled(true);
		ui.loginButton->setEnabled(true);
		//ui.logoutButton->setEnabled(false);
		ui.password->clear();

		this->done(DialogCode::Accepted);
	}else{

		this->done(DialogCode::Rejected);
	}
	*/
}

void ManualCheckin::setBTS(BTS_Sales *newbts){
	this->bts = newbts;
}

int ManualCheckin::get_return(){
	return this->checkin_return;
}