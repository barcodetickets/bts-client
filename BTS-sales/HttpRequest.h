#ifndef http_request
#define http_request
#include <QObject>
#include <QtNetwork/QtNetwork>

class HttpRequest : public QObject{
	Q_OBJECT

public:
	HttpRequest(QString url);
	~HttpRequest();

private slots:
	void finishedSlot(QNetworkReply* reply);
};

#endif
