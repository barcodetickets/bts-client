#include "BTS_Sales.h"
#include "logindialog.h"
#include "api.h"

#include <zbar/zbar.h>
#include <zbar/zbar/QZBar.h>
#include <QtGui>
#include <QtDebug>
#include <windows.h>
#include <iostream>
//#include <zbar/include/zbar/QZBar.h>

//BTS_Sales temp;


BTS_Sales::BTS_Sales(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	//BTS_Sales::inst = this;
	//temp = this;
	
	zbarPreview = new zbar::QZBar;
	zbarPreview->setObjectName(QString::fromUtf8("zbar"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(zbarPreview->sizePolicy().hasHeightForWidth());
	zbarPreview->setSizePolicy(sizePolicy);
	zbarPreview->setMinimumSize(QSize(320, 240));
	zbarPreview->setMaximumSize(QSize(640, 480));

	ui.verticalLayout_3->insertWidget(0, zbarPreview);
	
//	connect(zbarPreview, SIGNAL(decoded(int type, const QString&)),
//		this, SLOT(on_zbarPreview_decoded(int type, const QString&)));

	connect(zbarPreview, SIGNAL(decodedText(const QString&)),
		this, SLOT(on_zbarPreview_decodedText(const QString&)));
	
	connect(zbarPreview, SIGNAL(videoOpened(bool)),
		this, SLOT(on_zbarPreview_videoOpened(bool)));

	//ui.ticketID->append("sdfsdf");
	
	QStringList headerTitles;
	headerTitles << "Ticket #" << "Student #" << "Name";
	ui.recentTicketsList->setHeaderLabels(headerTitles);
	
	QStringList testRow, testRow2;
	testRow << "123" << "0987654321" << "John Doe";
	ui.recentTicketsList->addTopLevelItem(new QTreeWidgetItem(testRow));
	testRow2 << "124" << "8347246126" << "Jane Doe";
	ui.recentTicketsList->addTopLevelItem(new QTreeWidgetItem(testRow2));

	//ui.recentTicketsList->addColumn("Ticket #");
	//ui.recentTicketsList->addColumn("Student #");
	//ui.recentTicketsList->addColumn("Name");

	loginStatusIcon = new QLabel(statusBar());
	loginStatusIcon->setObjectName(QString::fromUtf8("loginStatusIcon"));
	loginStatusIcon->setMinimumSize(QSize(16, 16));
	loginStatusIcon->setPixmap(QPixmap(QString::fromUtf8(":/BTS_Sales/rsrc/disconnect.png")));

	ui.statusBar->addWidget(loginStatusIcon);

	QFont font1;
	loginStatusText = new QLabel(statusBar());
	loginStatusText->setObjectName(QString::fromUtf8("loginStatusText"));
	font1.setPointSize(8);
	loginStatusText->setFont(font1);
	loginStatusText->setText(tr("Not connected. Please log in."));
	ui.statusBar->addWidget(loginStatusText);
	loginStatusText->setLineWidth(3);

	console_open();

	QMap <QString, QString> map;
	map["password"] = "asdfqwer";
	map["username"] = "kirillp";
	api_request("/api/access/login/", map);
	//map["testparam"] = "sdfsdf";
	//map["asdf"] = "zzz";

	//signature_make("GET", "bts.example.com", "/api/access/login/", map, 
	//	"60026e856a7c93c1b459091a13e99db52315c6c97f5ecd1d3410c9f18ebccbb9");
}

BTS_Sales::~BTS_Sales()
{

}

void BTS_Sales::on_actionLogin_triggered(){
	LoginDialog d = new LoginDialog(this);
	d.show();

	if(d.exec() == QDialog::DialogCode::Accepted){
		loginStatusText->setText(tr("Logged in Successfully!"));
		loginStatusIcon->setPixmap(QPixmap::QPixmap(":/BTS_Sales/rsrc/accept.png"));
		ui.actionLogout->setEnabled(true);
		ui.actionLogin->setEnabled(false);
	}else{
		loginStatusText->setText(tr("Password or username incorrect."));
		loginStatusIcon->setPixmap(QPixmap::QPixmap(":/BTS_Sales/rsrc/cancel.png"));
	}

}

void BTS_Sales::on_actionLogout_triggered(){
	ui.actionLogout->setEnabled(false);
	ui.actionLogin->setEnabled(true);
	loginStatusIcon->setPixmap(QPixmap::QPixmap(":/BTS_Sales/rsrc/disconnect.png"));
	loginStatusText->setText(tr("Not connected. Please log in."));
}

void BTS_Sales::on_zbarPreview_videoOpened(bool videoOpened){
	if(videoOpened){
		ui.actionCapture->setEnabled(true);
	}
}

void BTS_Sales::on_zbarPreview_decodedText(const QString& data){
	if(data.startsWith(QString("QR-Code:"), Qt::CaseInsensitive)){
		QString asdf = data;
		asdf.remove(0, 8);
		ui.ticketID->setText(asdf);
	}
}

void BTS_Sales::on_actionCapture_triggered(){ 
	
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, NULL, 0, NULL);
	if(!zbarPreview->isVideoEnabled()){
		zbarPreview->setVideoDevice("/dev/video0");
		zbarPreview->setVideoEnabled(1);

		ui.actionCapture->setEnabled(zbarPreview->isVideoOpened());
		ui.actionCapture->setIcon(QIcon::QIcon(":/BTS_Sales/rsrc/stop.png"));
		ui.actionCapture->setText(QString("Stop Capture"));
	}else{
		zbarPreview->setVideoEnabled(0);
		ui.actionCapture->setIcon(QIcon::QIcon(":/BTS_Sales/rsrc/webcam.png"));
		ui.actionCapture->setText(QString("Start Capture"));
	}
}

Ui::BTS_SalesClass BTS_Sales::getUi(){
	return ui;
}
