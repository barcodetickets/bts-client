#ifndef api_header
#define api_header

#include <QtXml/QtXml>
#include "bts_sales.h"


#define STATUS_HTTP_OK				101
#define STATUS_HTTP_NOTFOUND		102
#define STATUS_HTTP_ERROR			103
#define STATUS_HTTP_ACCESSDENIED	103

#define STATUS_CONNECT_ERROR		104
#define STATUS_GENERAL_FAILURE		0xFFFF

#define STATUS_LOGIN_OK				0
#define STATUS_LOGIN_FAIL			1

#define STATUS_CHECKIN_OK			0
#define STATUS_CHECKIN_FAIL			1

#define STATUS_BARCODE_INACTIVE		0
#define STATUS_BARCODE_ACTIVE		1
#define STATUS_BARCODE_CHECKEDIN	2
#define STATUS_BARCODE_REFUNDED		3
#define STATUS_BARCODE_LOST_UNSOLD	4
#define STATUS_BARCODE_LOST_SOLD	5
#define STATUS_BARCODE_STOLEN		6
#define STATUS_BARCODE_UNLIMITED	254
#define STATUS_BARCODE_INVALIDX		255

void console_open();
QString signature_make(QString http_verb, QString hostname, QString url, QMap<QString, QString> params, QString api_key);

int api_request(QDomDocument *doc, QString api, QMap<QString, QString> params);
int api_login(QString username, QString password);
int api_logout();
int api_validate_barcode(QString data);
int api_checkin_barcode(QString data, BTS_Sales *bts);
int api_checkin(QString data, BTS_Sales *bts);
int api_checkin_name(QString first, QString last, int event_id, BTS_Sales *bts);
int api_checkin_id(QString id, int event_id, BTS_Sales *bts);

int config_load();
char *get_server_url();
char *api_get_eventid();


#endif