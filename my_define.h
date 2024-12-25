#ifndef MY_DEFINE_H
#define MY_DEFINE_H
#include <QString>
#include <QDebug>

#define API_ID     "10001671"
#define API_KEY    "HangZhoufogkniteleElectron950905"


#define URL_HOT_SEACH_WEIBO     "http://vip.apihz.cn/api/xinwen/weibo.php"
#define URL_HOT_SEACH_DOUYIN    "http://vip.apihz.cn/api/xinwen/douyinxingtujingxuan.php"
#define URL_PICTURE_BAIDU       "http://vip.apihz.cn/api/img/apihzimgbaidu.php"
#define URL_PICTURE_SOUGOU      "http://vip.apihz.cn/api/img/apihzimgsougou.php"
#define URL_ONE_WORD            "http://vip.apihz.cn/api/yiyan/api.php"
#define URL_IP_QUERY            "http://49.234.56.78/api/ip/getapi.php"
#define URL_HISTORY_TODAY       "http://vip.apihz.cn/api/time/getday.php"
#define URL_WEATHER_IP          "http://vip.apihz.cn/api/tianqi/tqybmoji15ip.php"

enum HTTP_TYPE {
    HOT_SEACH_WEIBO,
    HOT_SEACH_DOUYIN,
    PICTURE_BAIDU,
    PICTURE_SOUGOU,
    ONE_WORD,
    IP_QUERY,
    HISTORY_TODAY,
    WEATHER_IP,
    PICTURE_DOWNLOAD,
    UNKNOW_OPT //9
};
enum AUDIO_STATUS {
    NO_AUIO,
    PLAYING,
    PAUSE
};

struct WeatherData {
    QString week1;   // 星期（如：周二）
    QString week2;   // 日期（如：12/24）
    QString wea1;    // 白天气象（如：少云）
    QString wea2;    // 夜间天气（如：少云）
    QString wendu1;  // 白天温度（如：10°）
    QString wendu2;  // 夜间温度（如：5°）
};


struct S_Aduio_Meta {
    QString Title;
    QString Album;
    QString Artist;
    QString Duration;
    QString Position;
    QString Status;

    void print() const {
        qDebug() << "Title:" << Title;
        qDebug() << "Album:" << Album;
        qDebug() << "Artist:" << Artist;
        qDebug() << "Duration:" << Duration;
        qDebug() << "Position:" << Position;
        qDebug() << "Status:" << Status;

    }
};
struct S_HTTP_RESPONE {
    //QString Title;
    bool  success;
    QString Message;
    HTTP_TYPE Title;
    QString url;
    QByteArray bytes;
    QString Error;
    void print() const {
        //qDebug() << "Title:" << Title;
        qDebug() << "success:" << success;
        qDebug() << "Message:" << Message;
        qDebug() << "url:" << url;
        qDebug() << "Error:" << Error;

    }
};
#endif // MY_DEFINE_H
