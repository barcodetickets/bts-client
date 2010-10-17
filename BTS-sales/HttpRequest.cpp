#include <QUrl>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <stdlib.h>
#include <iostream>

#include "HttpRequest.h"

HttpRequest::HttpRequest(QString url){
	QNetworkAccessManager *nam = new QNetworkAccessManager();
	QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(finishedSlot(QNetworkReply*)));

	QNetworkReply *reply = nam->get(QNetworkRequest(QUrl(url)));
	
	std::cout << "[HttpRequest] URL: " << url.toStdString() + "\n";
}

HttpRequest::~HttpRequest(){

}

void HttpRequest::finishedSlot(QNetworkReply* reply){

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	std::cout << "HTTP response (" << statusCodeV.toInt() << ")\n";
	if(reply->error() == QNetworkReply::NoError){
		QByteArray data = reply->readAll();
		QString response(data);
		std::cout << "HTTP response (" << statusCodeV.toInt() << "): " << response.toStdString() << endl;
	}else{
		if(reply->error() == 202) std::cout << "error 403!!!\n";
		std::cout << "error " << (int)reply->error() + "\n";
	}
}