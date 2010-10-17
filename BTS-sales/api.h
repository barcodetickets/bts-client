#ifndef api_header
#define api_header

void console_open();
QString signature_make(QString http_verb, QString hostname, QString url, QMap<QString, QString> params, QString api_key);
void api_request(QString api, QMap<QString, QString> params);
int config_load();

#endif