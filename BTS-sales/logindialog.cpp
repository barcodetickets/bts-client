#include "logindialog.h"
#include "api.h"
#include <QtGui>
#include <windows.h>

LoginDialog::LoginDialog(QWidget *parent): QDialog(parent)
{
	ui.setupUi(this);
	ui.server->setText(get_server_url());
}

LoginDialog::~LoginDialog(){

}

void LoginDialog::on_loginButton_clicked(){
	
	int result = api_login(ui.username->text(), ui.password->text());
	if(HIWORD(result) == STATUS_HTTP_OK && LOWORD(result) == STATUS_LOGIN_OK){
		ui.username->setEnabled(false);
		ui.password->setEnabled(false);
		ui.loginButton->setEnabled(false);
		//ui.logoutButton->setEnabled(true);
		this->done(DialogCode::Accepted);
	}else if(HIWORD(result) != STATUS_HTTP_OK && HIWORD(result) != STATUS_HTTP_ACCESSDENIED){
		QMessageBox::information(this, "Connection Error", "Failed to communicate with server.");
	}else{
		ui.password->clear();
		this->done(DialogCode::Rejected);
	}
}

void LoginDialog::on_logoutButton_clicked(){
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
	
}