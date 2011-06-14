#include <QUrl>
#include <QtXml/QtXml>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <stdlib.h>
#include <iostream>

#include "HttpRequest.h"

HttpRequest::HttpRequest(QUrl url){
	QNetworkAccessManager *nam = new QNetworkAccessManager();
	QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(finishedSlot(QNetworkReply*)));
	
	this->reply = NULL;

	std::cout << "[HttpRequest] URL: " << url.toEncoded().data() << "\n";
	reply = nam->get(QNetworkRequest(url));
	

}

HttpRequest::~HttpRequest(){

}

void HttpRequest::finishedSlot(QNetworkReply* reply){
	this->reply = reply;
/*
	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	QByteArray data = reply->readAll();
	QString response(data);

	//std::cout << "HTTP response (" << statusCodeV.toInt() << ")\n";
	
	if(reply->error() == QNetworkReply::NoError){
		
		std::cout << "HTTP response (" << statusCodeV.toInt() << "): " << response.toStdString() << endl;
	}else{
		if(reply->error() == 202) std::cout << "error 403!!!\n";
		//std::cout << "error " << (int)reply->error() + "\n";
	}
	std::cout << response.toStdString() + "\n";

	QDomDocument doc("ResponseXML");
	doc.setContent(response);

	QDomElement root = doc.documentElement();
	QDomNode n = root.firstChild();
	
	while(!n.isNull()){
		QDomElement e = n.toElement();
		
		if(!e.isNull()){
			std::cout << e.tagName().toStdString() << ": " << e.text().toStdString() + "\n";
		}

		n = n.nextSibling();
	}
*/
}

QNetworkReply *HttpRequest::getNetworkReply(){
	return reply;
}