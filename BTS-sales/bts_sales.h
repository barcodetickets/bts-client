#ifndef BTS_Sales_H
#define BTS_Sales_H

#include <QtGui>
#include <QtGui/QMainWindow>
#include "ui_BTS_Sales.h"
#include <zbar/zbar.h>
#include <zbar/zbar/QZBar.h>

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
	void on_actionCapture_triggered();
	void on_zbarPreview_decodedText(const QString& data);
	void on_zbarPreview_videoOpened(bool videoOpened);
};

#endif // BTS_Sales_H
