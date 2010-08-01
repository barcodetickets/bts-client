#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include <QDialog>
#include "ui_logindialog.h"

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = 0);
	~LoginDialog();

private:
	Ui::LoginDialogClass ui;

private slots:
	void on_loginButton_clicked();
	void on_logoutButton_clicked();
};


#endif //LOGINDIALOG_H