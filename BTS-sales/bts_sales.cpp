#include "BTS_Sales.h"
#include "logindialog.h"
#include "manualcheckin.h"
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
	headerTitles << "Ticket #" << "Student Name" << "Time";
	ui.recentTicketsList->setHeaderLabels(headerTitles);
	
	
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
	
	ui.ticketID->setText("");
	ui.studentID->setText("");
	ui.firstName->setText("");
	ui.lastName->setText("");
	ui.barcodeStatusLabel->setText("Please log in.");
	ui.barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_RED);
	ui.ticketNum->setText("");
	ui.ticketStatus->setText("");

	//console_open();
	config_load();

	
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
		ui.checkInButton->setEnabled(true);
		ui.clearButton->setEnabled(true);
		//ui.actionManualCheckin->setEnabled(true);
		ui.actionCheckIn->setEnabled(true);
		ui.actionClear->setEnabled(true);
		ui.studentID->setFocus();

		ui.barcodeStatusLabel->setText("Ready to scan.");
		ui.barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_YELLOW);
	}else{
		loginStatusText->setText(tr("Password or username incorrect."));
		loginStatusIcon->setPixmap(QPixmap::QPixmap(":/BTS_Sales/rsrc/cancel.png"));
	}

}

void BTS_Sales::on_actionLogout_triggered(){
	if(!api_logout()){
		ui.actionLogout->setEnabled(false);
		ui.actionLogin->setEnabled(true);
		loginStatusIcon->setPixmap(QPixmap::QPixmap(":/BTS_Sales/rsrc/disconnect.png"));
		loginStatusText->setText(tr("Not connected. Please log in."));
		ui.checkInButton->setEnabled(false);
		ui.clearButton->setEnabled(false);
		//ui.actionManualCheckin->setEnabled(false);
		ui.actionCheckIn->setEnabled(false);
		ui.actionClear->setEnabled(false);
		
		ui.ticketID->setText("");
		ui.studentID->setText("");
		ui.firstName->setText("");
		ui.lastName->setText("");
		ui.barcodeStatusLabel->setText("Please log in.");
		ui.barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_RED);
		ui.ticketNum->setText("");
		ui.ticketStatus->setText("");
	}
}

void BTS_Sales::on_manualButton_clicked(){
	ManualCheckin d = new ManualCheckin(this);
	d.setBTS(this);
	d.show();

	if(d.exec() == QDialog::DialogCode::Accepted){
		
	}else{

	}
}

void BTS_Sales::on_checkInButton_clicked(){
	if(ui.ticketID->text().count("-") == 3){
		int result = api_checkin(ui.ticketID->text(), this);
	}else if(ui.studentID->text().compare("")){
		int result = api_checkin_id(ui.studentID->text(), QString(api_get_eventid()).toInt(), this);
	}else if(ui.firstName->text().compare("") && ui.lastName->text().compare("")){
		int result = api_checkin_name(ui.firstName->text(), ui.lastName->text(), QString(api_get_eventid()).toInt(), this);
	}
}

void BTS_Sales::on_clearButton_clicked(){
	ui.ticketID->setText("");
	ui.studentID->setText("");
	ui.firstName->setText("");
	ui.lastName->setText("");
	ui.ticketNum->setText("");
	ui.ticketStatus->setText("");
	ui.firstName->setFocus();
}

void BTS_Sales::on_zbarPreview_videoOpened(bool videoOpened){
	if(videoOpened){
		ui.actionCapture->setEnabled(true);
	}
}

void BTS_Sales::on_zbarPreview_decodedText(const QString& data){
	ui.ticketID->setText("");
	ui.studentID->setText("");
	ui.firstName->setText("");
	ui.lastName->setText("");
	ui.barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_YELLOW);
	ui.barcodeStatusLabel->setText("Scanning...");
	ui.ticketNum->setText("");
	ui.ticketStatus->setText("");

	if(data.startsWith(QString("QR-Code:"), Qt::CaseInsensitive)){
		QString asdf = data;
		asdf.remove(0, 8);
		ui.ticketID->setText(asdf);
		
		if(ui.radioValidate->isChecked()){
			int ret = api_validate_barcode(asdf);

			if(HIWORD(ret) == STATUS_HTTP_OK){
				switch(LOWORD(ret)){
					case STATUS_BARCODE_ACTIVE:{
						ui.barcodeStatusLabel->setText("Ticket is active. Validated!");
						QStringList newRow;
						newRow << asdf << "##########" << "John Doe";
						ui.recentTicketsList->addTopLevelItem(new QTreeWidgetItem(newRow));
						break;} 
					case STATUS_BARCODE_INACTIVE:
						ui.barcodeStatusLabel->setText("Inactive ticket!");
						break;
					case STATUS_BARCODE_CHECKEDIN:
						ui.barcodeStatusLabel->setText("Already checked in!");
						break;
					case STATUS_BARCODE_REFUNDED:
						ui.barcodeStatusLabel->setText("Ticket was refunded.");
						break;
					case STATUS_BARCODE_LOST_UNSOLD:
					case STATUS_BARCODE_LOST_SOLD:
						ui.barcodeStatusLabel->setText("Ticket was reported lost.");
						break;
					case STATUS_BARCODE_STOLEN:
						ui.barcodeStatusLabel->setText("Ticket was stolen.");
						break;
					case STATUS_BARCODE_INVALIDX:
						ui.barcodeStatusLabel->setText("Invalid ticket.");
						break;

				}
			}
		}else if(ui.radioCheckIn->isChecked()){
			int ret = api_checkin_barcode(asdf, this);
		}
	}else if(data.startsWith(QString("Code-39:"), Qt::CaseInsensitive)){
		QString asdf = data;
		asdf.remove(0, 8);
		if(ui.radioCheckIn->isChecked()){
			int ret = api_checkin_id(asdf, QString(api_get_eventid()).toInt(), this);
		}
	}
}

void BTS_Sales::on_actionCapture_triggered(){ 
	
	//int ret = api_validate_barcode("1;2;53mgvQWPech+IMfHG2iu46cgKGw33U5DILFlB+IW4xg=");
	//std::cout << ret << "\n";
	/*if(ui.radioCheckIn->isChecked()){
		//int ret = api_checkin_barcode("1;2;53mgvQWPech+IMfHG2iu46cgKGw33U5DILFlB+IW4xg=", this);
		int ret = api_checkin_barcode("1;2;e9MGtmPwlef2UUepZkz8NZ+0fJinyJeSTXyw2DNJKEo=", this);
	}
	return;*/
	
	//return;
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
