#include "BTS_Sales.h"
#include <QtGui/QApplication>

//#pragma comment(lib, "libzbarqt.dll.a")
#pragma comment(lib, "lib/qzbar.lib")
#pragma comment(lib, "lib/libzbar-0.lib")
#pragma comment(lib, "QtXml4.lib")
#pragma comment(lib, "QtNetwork4.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyleSheet("QStatusBar::item { border: 0px solid black }; ");

	BTS_Sales w;
	w.show();
	return a.exec();
}
