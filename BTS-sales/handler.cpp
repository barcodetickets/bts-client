#include "handler.h"
#include "BTS_Sales.h"
#include "zbar/include/zbar.h"
#include <QtGui>
#include <windows.h>

/*void MyHandler::image_callback (zbar::Image &image)
{
	for(zbar::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
		//QMessageBox::information(this, QObject::tr(symbol->get_data().c_str()), QObject::tr(symbol->get_type_name().c_str()));
		MessageBoxA(NULL, symbol->get_data().c_str(), symbol->get_type_name().c_str(), MB_OK);
		//ui.studentID->setText(tr(data));
	//cout << "decoded " << symbol->get_type_name() << " symbol "
	//<< "\"" << symbol->get_data() << "\"" << endl;
}*/

BTS_Sales *Handler::instance;

void Handler::setInstance(BTS_Sales *inst){
	instance = inst;
}

void Handler::barcode_callback(zbar::zbar_image_t *image, const void *userdata){
	const zbar::zbar_symbol_t *symbol = zbar_image_first_symbol(image);

	for(; symbol; symbol = zbar_symbol_next(symbol)) {
		/* do something useful with results */
		zbar::zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
		const char *data = zbar_symbol_get_data(symbol);
		instance->getUi().ticketID->setText(QObject::tr(data));
		//MessageBoxA(NULL, data, zbar_get_symbol_name(typ), MB_OK);
		//printf("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);
		//ui.ticketID->setText(tr(zbar_get_symbol_name(typ)));
		//ui.studentID->setText(tr(data));
	}
}
