#include "mainwindow.h"
#include "main_thread.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QProcess>
#include "httpclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonArray>
#include <QRegularExpression>
#include <unistd.h>
#include <QImage>
#include <QTimer>
#include <QTimerEvent>
#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

bool isImageWhite(QImage image, int threshold = 200)
{
    // QImage image(imagePath);
    if (image.isNull())
    {
        qWarning() << "Failed to load image.";
        return false;
    }

    int width = image.width();
    int height = image.height();
    int whitePixelCount = 0;
    int totalPixelCount = width * height;

    // 遍历所有像素并检查其亮度
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            QColor pixelColor(image.pixel(x, y));
            int red = pixelColor.red();
            int green = pixelColor.green();
            int blue = pixelColor.blue();

            // 计算亮度，使用标准加权公式
            int brightness = qRound(0.299 * red + 0.587 * green + 0.114 * blue);

            // 如果亮度大于设定的阈值，认为是接近白色的像素
            if (brightness >= threshold)
            {
                ++whitePixelCount;
            }
        }
    }
    float w_p = whitePixelCount / totalPixelCount;
    qDebug() << "while pwm : " << w_p << "whitePixelCount :" << whitePixelCount << "totalPixelCount" << totalPixelCount; //

    // 如果大部分像素是接近白色的，则认为图像是很白的
    return whitePixelCount > totalPixelCount * 0.75; // 假设90%以上的像素是接近白色的
}

int QprocessCommand(QString program, QString &output, QStringList arguments)
{
    // 设置启动的外部程序（比如运行 shell 命令 `echo`）
    // QString program = "echo";
    //    QStringList arguments;
    //    arguments << "Hello, QProcess!";

    QProcess process;
    // 启动外部进程
    process.start(program, arguments);

    // 等待进程执行完毕
    if (!process.waitForStarted())
    {
        qCritical() << "Failed to start the process";
        return -1;
    }

    // 等待进程执行完成
    if (!process.waitForFinished())
    {
        qCritical() << "Process did not finish correctly";
        return -1;
    }

    // 获取进程的标准输出
    output = process.readAllStandardOutput();
    // qDebug() << "Output:" << output;

    // 获取进程的返回值
    int exitCode = process.exitCode();
    // qDebug() << "Exit Code:" << exitCode;
    return exitCode;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //p_btmg->start();

    qDebug() << "****************************************************************";
    //sleep(3);
    wifiLaunch();
#if 0
    p_dbus = new qDbus();
    p_http = new HttpClient();
    connect(p_http, SIGNAL(HttpResult(S_HTTP_RESPONE)), this, SLOT(DisposeHttpResult(S_HTTP_RESPONE)), Qt::AutoConnection); // updatefile

    p_dbus->p_main = this;
    p_dbus->start();
    GetDeviceIP();
    GetOnewords();
    // QTimer *timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &MainWindow::DebugChache);
    // timer->start(5000);  // 每20ms更新一次（可根据需要调整旋转速度）
    label = new RotatingRoundLabel(80, this);  // 创建一个半径为100的圆形标签

//    MyGattService service;
//    service.createGattService();
#endif
 }

MainWindow::~MainWindow()
{
    delete ui;
}
// void MainWindow::DisposeHttpResult(S_HTTP_RESPONE  reply)
//{
//     qDebug() << data;
// }
void MainWindow::GetOnewords()
{

    QUrl url(URL_ONE_WORD);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    query.addQueryItem("words", "心灵鸡汤");

    url.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(url);
}

void MainWindow::GetDeviceIP()
{
    QUrl url(URL_IP_QUERY);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    url.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(url);
}

void MainWindow::GetWeatherOnIp()
{
    QUrl url(URL_WEATHER_IP);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    query.addQueryItem("ip", DeviceIP);

    url.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(url);
}

void MainWindow::DownloadAudioPctrue(QString geturl)
{
    geturl.replace("https", "http");
    QUrl imageUrl(geturl);
    QUrlQuery query;
    //    query.addQueryItem("id", API_ID);
    //    query.addQueryItem("key", API_KEY);

    imageUrl.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(imageUrl);
}

void MainWindow::disposeIP(S_HTTP_RESPONE s_back)
{
    // JSON 字符串
    QString jsonString = s_back.Message; //    R"({"code":200,"ip":"58.100.169.16","browser":"Unknown Browser","os":"Unknown OS"})";

    // 解析 JSON 字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 检查是否成功解析
    if (jsonDoc.isObject())
    {
        QJsonObject jsonObj = jsonDoc.object();

        // 提取 "ip" 字段
        QString ip = jsonObj.value("ip").toString();

        // 输出提取到的 IP 地址
        qDebug() << "IP Address:" << ip;
        DeviceIP = ip;
        ui->label_IP->setText(DeviceIP);
        GetWeatherOnIp();
    }
    else
    {
        qDebug() << "Invalid JSON data.";
    }
}

void MainWindow::disposeWeatherInfo(QString jsonString)
{
    p_http->printChache();


    QVector<WeatherData> weatherData = getWeatherData(jsonString, localPlace);
    QString showWeather;
    // 如果返回的天气数据不为空，则输出相关信息
    if (!weatherData.isEmpty())
    {
        qDebug() << "地点：" << localPlace;
        for (const WeatherData &data : weatherData)
        {
            qDebug() << "日期:" << data.week2 << data.week1;
            showWeather += "日期:" + data.week2 + data.week1 + "\n";
            qDebug() << "白天气象:" << data.wea1 << "夜间天气:" << data.wea2;
            showWeather += "白天气象:" + data.wea1 + "夜间天气:" + data.wea2 + "\n";
            qDebug() << "白天温度:" << data.wendu1 << "夜间温度:" << data.wendu2;
            showWeather += "白天温度:" + data.wendu1 + "夜间温度:" + data.wendu2 + "\n";
        }
    }
    else
    {
        qDebug() << "无法获取有效的天气数据！";
    }
    ui->label_weather->setText(showWeather);
}

void MainWindow::displayAudioMeta(S_Aduio_Meta *p_meta)
{
    ui->label_aumble->setText(p_meta->Album);

    ui->label_duration->setText(convertDurationToTimeFormat(p_meta->Duration));
    ui->label_position->setText(convertDurationToTimeFormat(p_meta->Position));
    ui->label_title->setText(p_meta->Title);
    // qDebug() << "USER PLAY CHANGED !" <<Playing_Album << ": " << p_meta->Album   << "boolean:" <<p_meta->Album.contains(Playing_Album);

    if (Playing_Album != p_meta->Album)
    {
        qDebug() << "USER PLAY CHANGED !" << Playing_Album << ": " << p_meta->Album << "boolean:" << p_meta->Album.contains(Playing_Album);

        //        if(p_meta->Artist.contains(Playing_Artist) || Playing_Artist =="")
        //        {

        Playing_Artist = p_meta->Artist;
        qDebug() << "Get picture";
        QString result = p_meta->Artist + "+" + p_meta->Album;
        GetAlbumPicture(result);

        Playing_Album = p_meta->Album;
    }
    ui->label_artist->setText(p_meta->Artist);
    if(p_meta->Status == "playing" && a_sta != PLAYING)
    {
        a_sta=PLAYING;
    }
    else if(p_meta->Status == "pause" && a_sta != PAUSE)
    {
        a_sta =PAUSE;
    }
}

void MainWindow::GetAlbumPicture(QString Artist)
{
    QUrl url(URL_PICTURE_BAIDU);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    // QRegularExpression re("[^a-zA-Z0-9]");
    // QString words ="Album art"+Artist;
    QString words = Artist.replace(" ", "");
    if (words.length() > 32)
    {
        words = words.left(32);
    }
    query.addQueryItem("words", words);
    url.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(QUrl(url));
}

/*HOT_SEACH_WEIBO,
HOT_SEACH_DOUYIN,
PICTURE_BAIDU,
PICTURE_SOUGOU,
ONE_WORD,
IP_QUERY,
HISTORY_TODAY,
WEATHER_IP*/
void MainWindow::DisposeHttpResult(S_HTTP_RESPONE s_back)
{
    qDebug() << "HTTP OPERATION:" << s_back.Title;
    if (s_back.Title == ONE_WORD)
    {
        DisposeOneWord(s_back);
    }
    else if (s_back.Title == PICTURE_BAIDU || s_back.Title == PICTURE_SOUGOU)
    {
        DisposePciteureJson(s_back);
    }
    else if (s_back.Title == PICTURE_DOWNLOAD)
    {
        // qDebug() << s_back.Message;
        QByteArray qmix = s_back.bytes;
        this->displayAlbumPicOnlabel(qmix);

    }
    else if (s_back.Title == IP_QUERY)
    {
        disposeIP(s_back);
    }
    else if (s_back.Title == WEATHER_IP)
    {
        disposeWeatherInfo(s_back.Message);
    }
    else
    {
        qDebug() << s_back.Title;
    }
}

void MainWindow::AlbumPicRotato()
{
}
void MainWindow::DebugChache()
{

    qDebug() << "udhcpc result : " ;
    p_http->sendGetRequest(QUrl("http://img0.baidu.com/it/u=4101285560,1286785277&fm=253&fmt=auto&app=120&f=JPEG"));

}
QString MainWindow::convertDurationToTimeFormat(const QString &durationStr)
{
    // 将输入的 QString 转换为整数（毫秒数）
    bool ok;
    int durationMs = durationStr.toInt(&ok);

    // 检查转换是否成功
    if (!ok)
    {
        return "00:00"; // 如果转换失败，返回默认值
    }

    // 将毫秒转换为秒
    int durationSec = durationMs / 1000;

    // 计算分钟和秒
    int minutes = durationSec / 60;
    int seconds = durationSec % 60;

    // 格式化为 "mm:ss" 格式
    return QString::asprintf("%02d:%02d", minutes, seconds);
}

void MainWindow::wifiLaunch()
{
//    QString res;
//    QStringList arguments;
//    arguments.clear();
//    QprocessCommand("wifi_daemon", res, arguments);
//    qDebug() << "wifi_daemon : " << res;
//    arguments.clear();
//    QprocessCommand("ifconfig", res, arguments);
//    qDebug() << "ifconfig : " << res;
//    arguments.clear();
//    arguments << "-o";
//    arguments << "sta";
//    QprocessCommand("wifi", res, arguments);
//    qDebug() << "wifi open : " << res;

//    arguments.clear();
//    arguments << "-t";
//    arguments << "ThanksGivingDay_111";
//    QprocessCommand("wifi", res, arguments);
//    qDebug() << "wifi -t : " << res;
    system("wifi_daemon\n");
    system("wifi -o sta\n");

}

void MainWindow::DisposeOneWord(S_HTTP_RESPONE s_back)
{
    QJsonDocument doc = QJsonDocument::fromJson(s_back.Message.toUtf8());

    // 如果解析成功，提取 msg 字段
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        QString msg = obj.value("msg").toString(); // 提取 msg 字段
        qDebug() << "msg字段的值:" << msg;
        ui->label_soulwords->setText(msg);
    }
    else
    {
        qDebug() << "无效的 JSON 格式";
    }
}

int MainWindow::DisposePciteureJson(S_HTTP_RESPONE s_back)
{
    qDebug() << s_back.Message;

    QString jsonString = s_back.Message;

    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject())
    {
        qDebug() << "Invalid JSON format!" << jsonString;
        return -1;
    }

    // 获取根对象
    QJsonObject jsonObj = doc.object();
    QStringList getUrl;
    // 判断 code 是否为 200
    if (jsonObj.value("code").toInt() == 200)
    {
        QJsonArray resArray = jsonObj.value("res").toArray();

        // 提取前5个URL
        int count = 0;
        for (const QJsonValue &value : resArray)
        {
            if (count < 5)
            {
                qDebug() << value.toString();
                getUrl.append(value.toString());
                count++;
            }
            else
            {
                break;
            }
        }
        DownloadAudioPctrue(getUrl.first());
    }
    else
    {
        qDebug() << "Code is not 200";
        // if (doc.object().value("msg").toString().contains("参数过长"))
        // {
          

            QString fixKeyword;
            if (Playing_Album.length() <= Playing_Artist.length())

                fixKeyword = Playing_Album;

            else

                fixKeyword = Playing_Artist;

            if (fixKeyword.length() >24)
            {

               fixKeyword= fixKeyword.left(24);
            }

            // QRegExp regex("[^A-Za-z0-9]");
            // fixKeyword= fixKeyword.replace(regex, "");
            qDebug() << "参数过长 校正后" <<fixKeyword;
            badKeywords= true;
            GetAlbumPicture(fixKeyword);
        // }
        // else
        // {
        //     GetAlbumPicture(Playing_Artist);
        // }
        sleep(1);
        return -1;
    }
}

void MainWindow::displayAlbumPicOnlabel(QByteArray bytes)
{
    QByteArray imageData = bytes;

    // 将下载的图片数据转换成 QImage
    QImage image;

    if (image.loadFromData(imageData))
    {
        if (isImageWhite(image, 200))
        {
            qDebug() << "this picture is not good ,try to refind one ";
            if (badKeywords)
            {
           
            QString fixKeyword;
            if (Playing_Album.length() <= Playing_Artist.length())

                fixKeyword = Playing_Album;

            else

                fixKeyword = Playing_Artist;

            if (fixKeyword.length() >16)
            {

               fixKeyword= fixKeyword.left(16);
            }

            // QRegExp regex("[^A-Za-z0-9]");
            // fixKeyword= fixKeyword.replace(regex, "");
            qDebug() << "参数过长 校正后" <<fixKeyword;
            badKeywords= true;
            GetAlbumPicture(fixKeyword+"+album");
            }
            else 
            GetAlbumPicture(Playing_Album+" 封面");
            return ;
        }

        // 将 QImage 转换为 QPixmap

        QPixmap pixmap = QPixmap::fromImage(image);


            
            label->move(530, 40);  // 设置标签位置


            label->loadImage(pixmap);  // 设置标签的图片
            label->show();

    }
    else
    {
        qWarning() << "图片加载失败";
    }
}

QVector<WeatherData> MainWindow::getWeatherData(const QString &jsonString, QString &place)
{
    QVector<WeatherData> weatherArray;

    // 解析JSON数据
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject())
    {
        qWarning() << "Invalid JSON format!";
        return weatherArray;
    }

    QJsonObject rootObj = doc.object();

    // 判断code码是否为200
    if (!rootObj.contains("code") || rootObj["code"].toInt() != 200)
    {
        qWarning() << "Invalid or unsuccessful response: code is not 200";
        return weatherArray; // 如果code不是200，返回空的结果
    }

    // 获取place信息
    if (rootObj.contains("place"))
    {
        place = rootObj["place"].toString();
    }
    else
    {
        place = "未知地点";
    }

    // 获取天气数据
    if (rootObj.contains("data"))
    {
        QJsonArray dataArray = rootObj["data"].toArray();

        // 获取前五天的天气数据
        for (int i = 0; i < 5 && i < dataArray.size(); ++i)
        {
            QJsonObject dayObj = dataArray[i].toObject();

            WeatherData data;
            data.week1 = dayObj["week1"].toString();
            data.week2 = dayObj["week2"].toString();
            data.wea1 = dayObj["wea1"].toString();
            data.wea2 = dayObj["wea2"].toString();
            data.wendu1 = dayObj["wendu1"].toString();
            data.wendu2 = dayObj["wendu2"].toString();

            weatherArray.append(data);
        }
    }

    return weatherArray;
}
