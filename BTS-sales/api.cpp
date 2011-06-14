#include "BTS_Sales.h"
#include "api.h"

#include <QUrl>
#include <QtNetwork/QtNetwork>
#include <QtXml/QtXml>


#include "httprequest.h"

#include <windows.h>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <io.h>

static QString hmac_sha1(const QString &key, const QString &secret);


char g_api_key[65];
char g_sys_name[65];
char g_event_id[4];

// length of server url cannot exceed 255 characters!
char g_server_url[256];

QString g_token;

// http://stackoverflow.com/questions/2629055/qtestlib-qnetworkrequest-not-executed
bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000) {
	QEventLoop loop;
	QTimer timer;
	timer.setInterval(timeout);
	timer.setSingleShot(true);

	loop.connect(sender, signal, SLOT(quit()));
	loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
	timer.start();
	loop.exec();

	return timer.isActive();
}

int api_request(QDomDocument *doc, QString api, QMap<QString, QString> params){
	QUrl request = QUrl(g_server_url + api + "?", QUrl::TolerantMode);
	std::cout << "dfsfsdfsdf\n";
	params["timestamp"] = QDateTime::currentDateTime().toUTC().toString("yyyyMMddhhmmss");
	params["sysName"] = g_sys_name;

	QMapIterator<QString, QString> i(params);
	while (i.hasNext()) {
		i.next();
		//QString val = i.value();
		//QUrl::toPercentEncoding(val);
		request.addEncodedQueryItem(QByteArray(i.key().toStdString().c_str()), QUrl::toPercentEncoding(i.value(), "", "+; "));
	}
	request.addEncodedQueryItem("signature", QUrl::toPercentEncoding(signature_make("GET", g_server_url, api, params, g_api_key), "", "+"));
	request.addQueryItem("format", "xml");

	std::cout << "[api_request] " + QString(request.toEncoded()).toStdString() + "\n";
	
	QNetworkAccessManager *nam = new QNetworkAccessManager();
	QNetworkReply *reply = nam->get(QNetworkRequest(request));

	waitForSignal(nam, SIGNAL(finished(QNetworkReply*)), 5000);
	
	if(!reply->isFinished()){
		// should never reach here ...
		reply->abort();
		std::cout << "!!!!!sdfsfSDUFYSDF(*&@!@#\n";
		return STATUS_CONNECT_ERROR;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	QByteArray data = reply->readAll();
	QString response(data);
	
	int err = STATUS_HTTP_OK;

	if(reply->error() == QNetworkReply::NoError){

		std::cout << "HTTP response (" << statusCodeV.toInt() << "): " << response.toStdString() << endl;
		switch(statusCodeV.toInt()){
			case 200:
				err = STATUS_HTTP_OK;
				break;
			case 401:
			case 500:
			case 503:
				err = STATUS_HTTP_ERROR;
				break;
			case 403:
				err = STATUS_HTTP_ACCESSDENIED;
			case 404:
				err = STATUS_HTTP_NOTFOUND;
				break;
		}
	}else{
		err = STATUS_GENERAL_FAILURE;
		switch(reply->error()){
			case QNetworkReply::ConnectionRefusedError:
			case QNetworkReply::RemoteHostClosedError:
			case QNetworkReply::TimeoutError:
			case QNetworkReply::OperationCanceledError:
				return STATUS_CONNECT_ERROR;
			case QNetworkReply::ContentNotFoundError:
			case QNetworkReply::HostNotFoundError:
				err = STATUS_HTTP_NOTFOUND;
				break;
			case QNetworkReply::ContentAccessDenied:
				err = STATUS_HTTP_ACCESSDENIED;
				break;
			case QNetworkReply::ContentOperationNotPermittedError:
				err = STATUS_HTTP_ERROR;
				break;
		}
		
	}
	std::cout << response.toStdString() + "\n";

	QDomDocument ret("ResponseXML");
	ret.setContent(response);
	

	delete reply;
	delete nam;

	if(doc) *doc = ret;

	return err;
}

int api_login(QString username, QString password){
	QMap <QString, QString> map;
	map["username"] = username;
	map["password"] = password;

	QDomDocument doc;
	int http = api_request(&doc, "/api/access/login/", map);
	
	if(doc.isNull() || http == STATUS_CONNECT_ERROR){
		return MAKELRESULT(STATUS_GENERAL_FAILURE, http);
	}

	QDomElement root = doc.documentElement();
	//QDomNode n = root.firstChild();

	QDomElement statusText = root.firstChildElement("statusText");
	if(!statusText.isNull()){
		printf("[api_login] found statusText node!!!\n");
		if(statusText.text().compare("OK")){
			printf("[api_login] bad user/password\n");
			return MAKELRESULT(STATUS_LOGIN_FAIL, STATUS_HTTP_ACCESSDENIED);
		}
	}

	QDomElement data = root.firstChildElement("data");
	if(!data.isNull()){
		QDomElement token = data.firstChildElement("token");
		if(!token.isNull()){
			g_token = token.text();
			std::cout << "[api_login] token found: " << g_token.toStdString() + "\n";
			return MAKELRESULT(STATUS_LOGIN_OK, http);
		}
	}

	return MAKELRESULT(STATUS_LOGIN_FAIL, http);
}

int api_logout(){
	QMap <QString, QString> map;
	map["token"] = g_token;
	
	std::cout << "[api_logout] fdsdfsdf!\n";
	// todo: actual error checking!
	if(api_request(NULL, "/api/access/logout/", map) != STATUS_HTTP_OK)
		return STATUS_GENERAL_FAILURE;
	
	std::cout << "[api_logout] success!\n";

	return 0;
}

int api_validate_barcode(QString data){
	QMap <QString, QString> map;
	map["barcode"] = data;
	map["token"] = g_token;

	QDomDocument doc;
	int http = api_request(&doc, "/api/tickets/validate-barcode/", map);
	

	if(doc.isNull() || http != STATUS_HTTP_OK) return MAKELRESULT(0, http);

	QDomElement root = doc.documentElement();
	
	
	QDomElement statusText = root.firstChildElement("statusText");
	if(!statusText.isNull()){
		printf("[api_login] found statusText node\n");
		if(statusText.text().compare("OK_VALID")){
			printf("[api_validate_barcode] fail");
			return MAKELRESULT(STATUS_GENERAL_FAILURE, http);
		}
	}

	QDomElement edata = root.firstChildElement("data");
	if(!data.isNull()){
		QDomElement ticketStatusCode = edata.firstChildElement("ticketStatusCode");
		if(!ticketStatusCode.isNull()){
			std::cout << "[api_validate_barcode] http code = " << http << "\n";
			std::cout << "[api_validate_barcode] return code = " << MAKELRESULT(ticketStatusCode.text().toStdString().at(0) - '0', http) << "\n";
			return MAKELRESULT(ticketStatusCode.text().toStdString().at(0) - '0', http);
		}
	}

	return MAKELRESULT(STATUS_GENERAL_FAILURE, http);
}

int api_checkin2(QString data, QDomDocument doc, BTS_Sales *bts, int http){

	QDomElement root = doc.documentElement();


	if(doc.isNull() || http != STATUS_HTTP_OK){
		QString errorStatus = "Failed Check-In";

		if(doc.isNull() || http == STATUS_CONNECT_ERROR){
			// an unrecoverable connection error occured

		}else{
			QDomElement statusText = root.firstChildElement("statusText");
			errorStatus = statusText.text();
		}


		bts->getUi().ticketID->setText("");
		bts->getUi().studentID->setText("");
		bts->getUi().firstName->setText("");
		bts->getUi().lastName->setText("");
		bts->getUi().barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_RED);
		bts->getUi().barcodeStatusLabel->setText("Check-in failed\n" + errorStatus);
		bts->getUi().lastCheckIn->setText("Last Check-In: never");
		bts->getUi().ticketNum->setText("");
		bts->getUi().ticketStatus->setText("");
		return MAKELRESULT(STATUS_GENERAL_FAILURE, http);
	}

	QDomElement edata = root.firstChildElement("data");
	if(!edata.isNull()){
		QDomElement attendee = edata.firstChildElement("attendee");
		QDomElement ticket = edata.firstChildElement("ticket");
		if(!attendee.isNull() && !ticket.isNull()){
			std::cout << "[api_checkin] attendee id: " << attendee.attribute("id").toStdString() << "\n";
			std::cout << "[api_checkin]        name: " << attendee.attribute("first_name").toStdString()
				<< " " << attendee.attribute("last_name").toStdString() << "\n";
			std::cout << "[api_checkin]    comments: " << attendee.attribute("comments").toStdString() << "\n";
			std::cout << "[api_checkin]       email: " << attendee.attribute("email").toStdString() << "\n";
			std::cout << "[api_checkin]      status: " << attendee.attribute("status").toStdString() << "\n";
			std::cout << "[api_checkin]   unique_id: " << attendee.attribute("unique_id").toStdString() << "\n";

			bts->getUi().ticketID->setText(data);
			bts->getUi().studentID->setText(attendee.attribute("unique_id"));
			bts->getUi().firstName->setText(attendee.attribute("first_name"));
			bts->getUi().lastName->setText(attendee.attribute("last_name"));

			bts->getUi().ticketNum->setText(ticket.attribute("ticket_id"));
			bts->getUi().ticketStatus->setText(ticket.attribute("status"));

			QString status;
			if(!ticket.attribute("status").compare("255")){
				status = "Unknown Failure";
			}else{
				switch(ticket.attribute("status").toStdString().at(0) - '0'){
					case STATUS_BARCODE_INACTIVE:
						status = "Inactive ticket!";
						break;
					case STATUS_BARCODE_CHECKEDIN:
						status = "Checked in at " + ticket.attribute("checkin_time").remove(0, 11);
						break;
					case STATUS_BARCODE_REFUNDED:
						status = "Ticket was refunded.";
						break;
					case STATUS_BARCODE_LOST_UNSOLD:
					case STATUS_BARCODE_LOST_SOLD:
						status = "Ticket was reported lost.";
						break;
					case STATUS_BARCODE_STOLEN:
						status = "Ticket was stolen.";
						break;
					default:
						status = "???";
						break;
				}
			}

			QDomElement statusText = root.firstChildElement("statusText");
			if(!statusText.isNull()){
				printf("[api_checkin] found statusText node\n");
				if(!statusText.text().compare("OK_CHECKED_IN")){
					printf("[api_checkin] valid ticket - checked in!\n");
					if(!attendee.isNull() && !ticket.isNull()){

						QStringList newRow;
						newRow << ticket.attribute("ticket_id") << attendee.attribute("first_name") + " " + attendee.attribute("last_name") << QTime::currentTime().toString("hh:mm:ss");

						QTreeWidgetItem *newItem = new QTreeWidgetItem(newRow);
						bts->getUi().recentTicketsList->addTopLevelItem(newItem);
						bts->getUi().recentTicketsList->addTopLevelItem(newItem);
						bts->getUi().recentTicketsList->addTopLevelItem(newItem);
						bts->getUi().recentTicketsList->sortByColumn(2, Qt::DescendingOrder);
						bts->getUi().recentTicketsList->setCurrentItem(newItem);

						bts->getUi().lastCheckIn->setText("Last Check-In: " + QTime::currentTime().toString("hh:mm:ss"));
						bts->getUi().barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_GREEN);
						bts->getUi().barcodeStatusLabel->setText(attendee.attribute("first_name") + " " + attendee.attribute("last_name") + "\nChecked in!");
						return MAKELRESULT(0, http);
					}
				}else{
					bts->getUi().barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_RED);
					bts->getUi().barcodeStatusLabel->setText("Check-in failed (" + attendee.attribute("first_name") + " " + attendee.attribute("last_name") + ")\n"+ status);
				}
			}

		}else{
			printf("[api_checkin] no such attendee\n");
			bts->getUi().ticketID->setText("");
			bts->getUi().studentID->setText("");
			bts->getUi().firstName->setText("");
			bts->getUi().lastName->setText("");
			bts->getUi().barcodeStatusLabel->setStyleSheet(GUI_STATUS_COLOR_RED);
			bts->getUi().barcodeStatusLabel->setText("Check-in failed\nTicket not sold");
			bts->getUi().lastCheckIn->setText("Last Check-In: never");
			bts->getUi().ticketNum->setText("");
			bts->getUi().ticketStatus->setText("");
		}
	}else{
		// no <data> was given ... something stupid happen
		// TODO: do something here!
	}
	return MAKELRESULT(STATUS_GENERAL_FAILURE, http);
}

int api_checkin_barcode(QString data, BTS_Sales *bts){
	QMap <QString, QString> map;
	map["barcode"] = data;
	map["token"] = g_token;

	QDomDocument doc;
	int http = api_request(&doc, "/api/tickets/check-in-barcode/", map);

	return api_checkin2(data, doc, bts, http);
}

int api_checkin(QString data, BTS_Sales *bts){
	QMap <QString, QString> map;
	
	QStringList params = data.split("-");

	map["event"]	= params.at(0);
	map["batch"]	= params.at(1);
	map["ticket"]	= params.at(2);
	map["checksum"]	= params.at(3);
	map["token"] = g_token;

	QDomDocument doc;
	int http = api_request(&doc, "/api/tickets/check-in/", map);

	return api_checkin2(data, doc, bts, http);
}

int api_checkin_name(QString first, QString last, int event_id, BTS_Sales *bts){
	QMap <QString, QString> map;

	map["firstName"] = first;
	map["lastName"]	= last;
	map["token"] = g_token;

	QDomDocument doc;
	int http = api_request(&doc, "/api/attendees/find/", map);
	
	if(http == STATUS_HTTP_NOTFOUND){
		// no attendee was found!
		QMessageBox::information(bts, "Attendee Not Found", "This attendee does not exist! Check spelling of name?");
		return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
	}
	
	QDomElement root = doc.documentElement();
	
	QDomElement edata = root.firstChildElement("data");
	if(!edata.isNull()){
		QDomElement attendee = edata.firstChildElement("attendee");
		if(!attendee.isNull()){
			std::cout << "[api_checkin_name] found an attendee!\n";
			QDomElement ticket = attendee.firstChildElement("ticket");

			while(!ticket.isNull()){
				// ok so a ticket was found
				//printf("[api_checkin_name] debug: event id: %d || %d\n", event_id, ticket.attribute("event_id").toInt());
				if(ticket.attribute("event_id").toInt() == event_id && 
					(ticket.attribute("status").toInt() == 1 || ticket.attribute("status").toInt() == 254)){
					std::cout << "[api_checkin_name] found a ticket for this attendee! id: " + ticket.attribute("ticket_id").toStdString() + "\n";
					return api_checkin(ticket.attribute("event_id") + "-" +
							ticket.attribute("batch") + "-" +
							ticket.attribute("ticket_id") + "-" +
							ticket.attribute("checksum"), bts);
				}else{
					ticket = ticket.nextSiblingElement();
				}
			}
			// no ticket was found!
			QMessageBox::information(bts, "Ticket Not Found", "This attendee does not have a ticket to this event!");
			return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
		}else{
			// no attendee was found!
			QMessageBox::information(bts, "Attendee Not Found", "This attendee does not exist! Check spelling of name?");
			return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
		}
	}else{
		// ???
		QMessageBox::information(bts, "Error", "A server error probably occured.");
		return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
	}
	return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
}


int api_checkin_id(QString id, int event_id, BTS_Sales *bts){
	QMap <QString, QString> map;

	map["uniqueId"] = id;
	map["token"] = g_token;

	QDomDocument doc;
	int http = api_request(&doc, "/api/attendees/find/", map);

	if(http == STATUS_HTTP_NOTFOUND){
		// no attendee was found!
		QMessageBox::information(bts, "Attendee Not Found", "This attendee does not exist! Check spelling of name?");
		return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
	}

	QDomElement root = doc.documentElement();

	QDomElement edata = root.firstChildElement("data");
	if(!edata.isNull()){
		QDomElement attendee = edata.firstChildElement("attendee");
		if(!attendee.isNull()){
			std::cout << "[api_checkin_name] found an attendee!\n";
			QDomElement ticket = attendee.firstChildElement("ticket");

			while(!ticket.isNull()){
				// ok so a ticket was found
				printf("[api_checkin_name] debug: event id: %d || %d\n", event_id, ticket.attribute("event_id").toInt());
				if(ticket.attribute("event_id").toInt() == event_id && 
					(ticket.attribute("status").toInt() == 1 || ticket.attribute("status").toInt() == 254)){
						std::cout << "[api_checkin_name] found a ticket for this attendee! id: " + ticket.attribute("ticket_id").toStdString() + "\n";
						return api_checkin(ticket.attribute("event_id") + "-" +
							ticket.attribute("batch") + "-" +
							ticket.attribute("ticket_id") + "-" +
							ticket.attribute("checksum"), bts);
				}else{
					ticket = ticket.nextSiblingElement();
				}
			}
			// no ticket was found!
			QMessageBox::information(bts, "Ticket Not Found", "This attendee does not have a ticket to this event!");
			return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
		}else{
			// no attendee was found!
			QMessageBox::information(bts, "Attendee Not Found", "This attendee does not exist! Check spelling of name?");
			return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
		}
	}else{
		// ???
		QMessageBox::information(bts, "Error", "A server error probably occured.");
		return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
	}
	return MAKELRESULT(STATUS_CHECKIN_FAIL, http);
}

QString signature_make(QString http_verb, QString hostname, QString url, QMap<QString, QString> params, QString api_key){
	QString to_sign = http_verb + " " + hostname.remove(0, 7) + url + "\n";
	QUrl request = QUrl("", QUrl::TolerantMode);
	QMapIterator<QString, QString> i(params);
	while (i.hasNext()) {
		i.next();
		//QString val = i.value();
		//QUrl::toPercentEncoding(val);
		//to_sign += i.key() + "=" +  QUrl::toPercentEncoding(i.value(), "", " +;");
		request.addEncodedQueryItem(QByteArray(i.key().toStdString().c_str()), QUrl::toPercentEncoding(i.value(), "", "+; "));
		//request.addQueryItem(i.key(), i.value());
		//if(i.hasNext()) to_sign += "&";
	}
	
	QString str_req(request.toEncoded());
	to_sign += str_req.remove(0, 1);

	std::cout << to_sign.toStdString() << "\n";
	QString sig = hmac_sha1(to_sign, api_key);

	//QUrl::toPercentEncoding(sig);
	std::cout << "[signature_make] " << sig.toStdString() << "\n";
	return sig;
}

int config_load(){
	char path[MAX_PATH + 16];

	GetCurrentDirectoryA(MAX_PATH, path);
	strcat_s(path, MAX_PATH + 16, "\\bts.ini");
	printf("[config_load] ini path: %s\n", path);

	GetPrivateProfileStringA("system", "api_key", "undefined", g_api_key, 65, path);
	printf("[config_load] api_key = %s\n", g_api_key);

	GetPrivateProfileStringA("system", "sys_name", "undefined", g_sys_name, 65, path);
	printf("[config_load] sys_name = %s\n", g_sys_name);

	GetPrivateProfileStringA("system", "server_location", "undefined", g_server_url, 256, path);
	printf("[config_load] server_url = %s\n", g_server_url);

	GetPrivateProfileStringA("system", "event_id", "undefined", g_event_id, 4, path);
	printf("[config_load] batch_id = %s\n", g_event_id);


	return 0;
}

char *api_get_eventid(){
	return g_event_id;
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

char *get_server_url(){
	return g_server_url;
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
