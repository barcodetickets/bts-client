#ifndef BTS_Sales_H
#define BTS_Sales_H

#include <QtGui>
#include <QtGui/QMainWindow>
#include "ui_BTS_Sales.h"
#include <zbar/zbar.h>
#include <zbar/zbar/QZBar.h>

#define GUI_STATUS_COLOR_GREEN  "background-color: rgb(26, 99, 13);\ncolor: rgb(255, 255, 255);"
#define GUI_STATUS_COLOR_YELLOW "background-color: rgb(203, 161, 33);\ncolor: rgb(255, 255, 255);"
#define GUI_STATUS_COLOR_RED    "background-color: rgb(151, 36, 38);\ncolor: rgb(255, 255, 255);"

class BTS_Sales : public QMainWindow
{
	Q_OBJECT

public:
	BTS_Sales(QWidget *parent = 0, Qt::WFlags flags = 0);
	~BTS_Sales();
	//static BTS_Sales *inst;
	Ui::BTS_SalesClass getUi();
	//static void barcode_callback(zbar::zbar_image_t *image, const void *userdata);

private:
	QLabel *loginStatusIcon;
	QLabel *loginStatusText;
	Ui::BTS_SalesClass ui;
	zbar::QZBar *zbarPreview;
	//MyHandler handler;

private slots:
	void on_actionLogin_triggered();
	void on_actionLogout_triggered();
	void on_manualButton_clicked();
	void on_clearButton_clicked();
	void on_checkInButton_clicked();

	void on_actionCapture_triggered();
	void on_zbarPreview_decodedText(const QString& data);
	void on_zbarPreview_videoOpened(bool videoOpened);
};

#endif // BTS_Sales_H
