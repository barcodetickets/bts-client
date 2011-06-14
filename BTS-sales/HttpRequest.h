#ifndef http_request
#define http_request
#include <QObject>
#include <QtNetwork/QtNetwork>

class HttpRequest : public QObject{
	Q_OBJECT

public:
	HttpRequest(QUrl url);
	~HttpRequest();
	QNetworkReply *getNetworkReply();
private:
	QNetworkReply *reply;

private slots:
	void finishedSlot(QNetworkReply* reply);
};

#endif
