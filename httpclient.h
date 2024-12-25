#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include "my_define.h"
class HttpClient : public QObject
{
    Q_OBJECT

public:
    explicit HttpClient(QObject *parent = nullptr);
    void sendGetRequest(QUrl url);
    void sendPostRequest(QUrl url);

private slots:
    void onFinished(QNetworkReply *reply);
signals:
    void HttpResult(S_HTTP_RESPONE);
private:
    QNetworkAccessManager *networkManager;
};

#endif // HTTPCLIENT_H
