#include "logindialog.h"
#include <QtGui>

LoginDialog::LoginDialog(QWidget *parent): QDialog(parent)
{
	ui.setupUi(this);
	ui.server->setText("http://bts.rhhsstuco.org/public/");
}

LoginDialog::~LoginDialog(){

}

void LoginDialog::on_loginButton_clicked(){
	if(!ui.password->text().compare("asdfqwer")){
		ui.username->setEnabled(false);
		ui.password->setEnabled(false);
		ui.loginButton->setEnabled(false);
		//ui.logoutButton->setEnabled(true);
		this->done(DialogCode::Accepted);
	}else{
		
		ui.password->clear();
		this->done(DialogCode::Rejected);
	}
}

void LoginDialog::on_logoutButton_clicked(){
	ui.username->setEnabled(true);
	ui.password->setEnabled(true);
	ui.loginButton->setEnabled(true);
	//ui.logoutButton->setEnabled(false);
	ui.password->clear();

	this->done(DialogCode::Accepted);
}