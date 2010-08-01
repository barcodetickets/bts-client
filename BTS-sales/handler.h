#ifndef HANDLER_H
#define HANDLER_H

#include "BTS_Sales.h"
#include "zbar/include/zbar.h"

class Handler
{
public:
	static void barcode_callback (zbar::zbar_image_t *image, const void *userdata);
	static void setInstance(BTS_Sales *inst);
private:
	static BTS_Sales *instance;
};

#endif