#include "BTS_Sales.h"
#include "api.h"

#include <QUrl>
#include <QtNetwork/QtNetwork>

#pragma comment(lib, "QtNetwork4.lib")

#include "httprequest.h"

#include <windows.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <io.h>

static QString hmac_sha1(const QString &key, const QString &secret);

char *api_key = "24e8a7c5cd1087083c9c8d5a378ba102496a503b96fbd4fbbe21e452bcbdba7b";
char *g_url = "http://bts.rhhsstuco.org/public";
char *sys_name = "testing";

void api_request(QString api, QMap<QString, QString> params){
	QString request = g_url + api + "?";
	
	params["timestamp"] = QDateTime::currentDateTime().toUTC().toString("yyyyMMddhhmmss");
	params["sysName"] = sys_name;

	QMapIterator<QString, QString> i(params);
	while (i.hasNext()) {
		i.next();
		//QString val = i.value();
		//QUrl::toPercentEncoding(val);
		request += i.key() + "=" + QUrl::toPercentEncoding(i.value());
		request += "&";
	}
	
	request += "signature=" + signature_make("GET", g_url, api, params, api_key);
	request += "&format=xml";
	
	std::cout << request.toStdString() + "\n";
	HttpRequest *a = new HttpRequest(request);
	//a->deleteLater();
	//delete a;
/*
	QNetworkAccessManager *nam = new QNetworkAccessManager();
	QNetworkReply *reply = nam->get(QNetworkRequest(QUrl(request)));
	reply->waitForReadyRead(-1);
	if(reply->error() == QNetworkReply::NoError){
		QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		QByteArray data = reply->readAll();
		QString response(data);
		std::cout << "HTTP response (" << statusCodeV.toInt() << "): " << response.toStdString() << endl;
	}else{
		std::cout << "HTTP error " << reply->error() + "\n";
	}*/
	
	//QUrl url(request);
	//QNetworkReply* reply = nam->get(QNetworkRequest(url));
}

QString signature_make(QString http_verb, QString hostname, QString url, QMap<QString, QString> params, QString api_key){
	QString to_sign = http_verb + " " + hostname.remove(0, 7) + url + "\n";

	QMapIterator<QString, QString> i(params);
	while (i.hasNext()) {
		i.next();
		//QString val = i.value();
		//QUrl::toPercentEncoding(val);
		to_sign += i.key() + "=" + QUrl::toPercentEncoding(i.value());
		if(i.hasNext()) to_sign += "&";
	}
	
	std::cout << to_sign.toStdString() << "\n";
	QString sig = QUrl::toPercentEncoding(hmac_sha1(to_sign, api_key));

	//QUrl::toPercentEncoding(sig);
	std::cout << "[signature_make] " << sig.toStdString() << "\n";
	return sig;
}



/**
 * Hashes the given string using the HMAC-SHA1 algorithm.
 *
 * http://stackoverflow.com/questions/3341167/how-to-implement-hmac-sha1-algorithm-in-qt/3594062#3594062
 * 
 * \param key The string to be hashed
 * \param secret The string that contains secret word
 * \return The hashed string
 */
static QString hmac_sha1(const QString &key, const QString &secret)
{
    int text_length; // Length of the text, that will be hashed
    unsigned char* K; //For secret word.
    int K_length; //Length of secret word

    K_length = secret.size();
    text_length = key.size();

    //Need to do for XOR operation. Transforms QString to unsigned char
    std::string tempString = secret.toStdString();
    K = (unsigned char*)tempString.c_str();

    unsigned char ipad[65]; // Inner pad
    unsigned char opad[65]; // Outer pad

    //unsigned char L[20]; //TODO if key > 64 bytes use this to obtain sha1 key

    // Fills ipad and opad with zeros
    memset( ipad, 0, sizeof ipad);
    memset( opad, 0, sizeof opad);

    // Copies Secret to ipad and opad
    memcpy( ipad, K, K_length);
    memcpy( opad, K, K_length);

    // XOR operation for inner and outer pad
    for (int i=0; i<64; i++) {
        ipad[i] ^= 0x36;
        opad[i] ^= 0x5c;
    }

    QByteArray context; // Stores hashed content

    context.append((const char*) ipad,64); // Appends XOR:ed ipad to context
    context.append(key); //Appends key to context

    //Hashes Inner pad
    QByteArray Sha1 = QCryptographicHash::hash(context,
                QCryptographicHash::Sha1);

    context.clear();
    context.append((const char*) opad,64); //Appends opad to context
    context.append(Sha1); //Appends hashed inner pad to context

    Sha1.clear();

    // Hashes outerpad
    Sha1 = QCryptographicHash::hash(context, QCryptographicHash::Sha1);

    // String to return hashed stuff in Base64 format
    QByteArray str(Sha1.toBase64());

    return str;
}


void console_open(){
	int outHandle, errHandle, inHandle;
	FILE *outFile, *errFile, *inFile;
	AllocConsole();
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 9999;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE),_O_TEXT);
	inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE),_O_TEXT );

	outFile = _fdopen(outHandle, "w" );
	errFile = _fdopen(errHandle, "w");
	inFile =  _fdopen(inHandle, "r");

	*stdout = *outFile;
	*stderr = *errFile;
	*stdin = *inFile;

	setvbuf( stdout, NULL, _IONBF, 0 );
	setvbuf( stderr, NULL, _IONBF, 0 );
	setvbuf( stdin, NULL, _IONBF, 0 );

	std::ios::sync_with_stdio();

}
