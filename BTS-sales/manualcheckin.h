#ifndef MANUALCHECKIN_H
#define MANUALCHECKIN_H
#include <QDialog>
#include "bts_sales.h"
#include "ui_manualcheckin.h"

class ManualCheckin : public QDialog
{
	Q_OBJECT

public:
	ManualCheckin(QWidget *parent = 0);
	~ManualCheckin();
	void setBTS(BTS_Sales *newbts);

private:
	Ui::ManualCheckin ui;
	int checkin_return;
	int get_return();
	BTS_Sales *bts;

	private slots:
		void on_okButton_clicked();
		void on_CancelButton_clicked();
};


#endif //MANUALCHECKIN_H