#include "httpclient.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
HttpClient::HttpClient(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this))
{
    // 连接网络请求完成的信号
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &HttpClient::onFinished);
}

void HttpClient::sendGetRequest( QUrl url)
{
    // 创建 GET 请求的 URL
   //("https://jsonplaceholder.typicode.com/posts/1");
    QNetworkRequest request(url);

    // 发送 GET 请求
    networkManager->get(request);
}

void HttpClient::sendPostRequest(QUrl url)
{
   // QUrl url("https://jsonplaceholder.typicode.com/posts");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 构造 JSON 数据
    QJsonObject json;
    json["title"] = "foo";
    json["body"] = "bar";
    json["userId"] = 1;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // 发送 POST 请求
    networkManager->post(request, data);
}
void HttpClient::printChache()
{
    if (p_test != NULL)
    {
      qDebug() <<"Monitor Chache status:\n";
      qDebug() << p_test->bytes.length();
      //p_test->print();
    }
    
}


void HttpClient::onFinished(QNetworkReply *reply)
{
    S_HTTP_RESPONE GetRespone;
    qDebug() << "operation:" << reply->operation();
    qDebug() << "url:" << reply->url();
    qDebug() << "size:" << reply->size();
    
    // 状态码
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "status code:" << statusCode;

    if (reply->error() == QNetworkReply::NoError) {
        // 判断响应内容类型
        if(reply->url().toString().contains("sogoucdn") || reply->url().toString().contains("baidu.com"))
        {
            GetRespone.Title = PICTURE_DOWNLOAD;
            GetRespone.success = true;
            QByteArray reply_data = reply->readAll();
            GetRespone.bytes = reply_data;
            qDebug() << "getpicture size " << reply_data.length();
            qDebug() << "status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            p_test = &GetRespone;
        }   
        else
        {
            // 请求成功，输出结果
            QString response = reply->readAll();
            GetRespone.success = true;
            GetRespone.Message = response;

            qDebug() << reply->url().toString() << "debug : " << reply->url().toString().contains(URL_ONE_WORD);
            GetRespone.url = reply->url().toString();

            // 根据不同的URL，设置响应标题
            if(reply->url().toString().contains(URL_HOT_SEACH_WEIBO))
                GetRespone.Title = HOT_SEACH_WEIBO;
            else if(reply->url().toString().contains(URL_HOT_SEACH_DOUYIN))
                GetRespone.Title = HOT_SEACH_DOUYIN;
            else if(reply->url().toString().contains(URL_PICTURE_BAIDU))
                GetRespone.Title = PICTURE_BAIDU;
            else if(reply->url().toString().contains(URL_PICTURE_SOUGOU))
                GetRespone.Title = PICTURE_SOUGOU;
            else if(reply->url().toString().contains(URL_ONE_WORD))
                GetRespone.Title = ONE_WORD;
            else if(reply->url().toString().contains(URL_IP_QUERY))
                GetRespone.Title = IP_QUERY;
            else if(reply->url().toString().contains(URL_HISTORY_TODAY))
                GetRespone.Title = HISTORY_TODAY;
            else if(reply->url().toString().contains(URL_WEATHER_IP)) // i03piccdn.sogoucdn.com
                GetRespone.Title = WEATHER_IP;
            else if(reply->url().toString().contains(URL_HOTSEARCH)) // i03piccdn.sogoucdn.com
                GetRespone.Title = HOTSEARCH;
            else if(reply->url().toString().contains(URL_DATETODAY)) // i03piccdn.sogoucdn.com
                GetRespone.Title = DATETODAY;
            else if(reply->url().toString().contains(URL_WEATHERTODAY)) // i03piccdn.sogoucdn.com
                GetRespone.Title = WEATHERTODAY;
            else
                GetRespone.Title = UNKNOW_OPT;
        }
    }
    else {
        // 请求失败，输出错误信息
        qDebug() << "Error:" << reply->errorString();
        GetRespone.Error = reply->errorString();
        GetRespone.success = false;
    //    emit HttpError(GetRespone);

    //    return ;
    }

    // 确保 GetRespone 是通过信号传递
    emit HttpResult(GetRespone);

    // 正确删除 QNetworkReply
    reply->deleteLater();
}
