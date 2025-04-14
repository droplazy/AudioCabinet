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
#include "fingerthread.h"
#include "bt_manager.h"
#include <QJsonObject>
#include <QJsonArray>

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

    qDebug() << "****************************************************************";
    // sleep(3);
    wifiLaunch();

    p_http = new HttpClient();
    p_thread = new main_thread();
    p_thread->start();
    p_finger = new FingerThread();
    p_finger->start();
    connect(p_http, SIGNAL(HttpResult(S_HTTP_RESPONE)), this, SLOT(DisposeHttpResult(S_HTTP_RESPONE)), Qt::AutoConnection); //
    connect(p_thread, SIGNAL(wlanConnected()), this, SLOT(flushNetUI()), Qt::AutoConnection);                               // updateAudioTrack
    connect(p_thread, SIGNAL(updateAudioTrack()), this, SLOT(displayAudioMeta()), Qt::AutoConnection);                      //
    connect(p_thread, SIGNAL(DebugSignal()), this, SLOT(UserAddFinger()), Qt::AutoConnection);                              //
    connect(p_finger, SIGNAL(upanddownlock()), this, SLOT(ElcLockOption()), Qt::AutoConnection);                            //

#if RotationLabel

    label = new RotatingRoundLabel(80, this); // 创建一个半径为100的圆形标签

#endif
    setPlayProgress(0);
    QTimer *timer = new QTimer();
    /* connect(timer, &QTimer::timeout, this, [](){
         qDebug() << "Timeout aaatriggered!";
       //  PULLDOWN_ELCLOCK;
     });*/
    connect(timer, &QTimer::timeout, this, &MainWindow::displaySpectrum);
    timer->start(5); // 每20ms更新一次（可根据需要调整旋转速度）
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
void MainWindow::GetHotSearch()
{

    QUrl url(URL_HOTSEARCH);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
  //  query.addQueryItem("words", "心灵鸡汤");

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
void MainWindow::GetDateToday()
{
    QUrl url(URL_DATETODAY);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    query.addQueryItem("type", "1");

    url.setQuery(query); // 设置 URL 的查询部分
    p_http->sendGetRequest(url);
}
void MainWindow::GetWeatherToday()
{
    QUrl url(URL_WEATHERTODAY);
    QUrlQuery query;
    query.addQueryItem("id", API_ID);
    query.addQueryItem("key", API_KEY);
    query.addQueryItem("ip", "101.68.34.143");

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
        //ui->label_IP->setText(DeviceIP);
        GetWeatherOnIp();
        GetWeatherToday();
    }
    else
    {
        qDebug() << "Invalid JSON data.";
    }
}

void MainWindow::disposeWeatherInfo(QString jsonString)
{
    p_http->printChache();
    qDebug() << jsonString;
    // 解析 JSON 数据
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON data.";
        return;
    }

    QJsonObject jsonObj = doc.object();
    QJsonArray dataArray = jsonObj["data"].toArray();

    // 获取当前日期、明天和后天的日期
    QDateTime currentDate =dateTime;
    QDateTime tomorrow = currentDate.addDays(1);
    QDateTime dayAfterTomorrow = currentDate.addDays(2);

    // 获取日期字符串，格式化为"MM/dd"
    QString currentDateString = currentDate.toString("MM/dd");
    QString tomorrowDateString = tomorrow.toString("MM/dd");
    QString dayAfterTomorrowDateString = dayAfterTomorrow.toString("MM/dd");

    // 遍历数据，找出今天、明天和后天的天气
    QString todayMaxTemp, todayMinTemp;
    QString tomorrowMaxTemp, tomorrowMinTemp;
    QString dayAfterTomorrowMaxTemp, dayAfterTomorrowMinTemp;

    for (const QJsonValue& value : dataArray) {
        QJsonObject dayObj = value.toObject();
        QString date = dayObj["week2"].toString();

        if (date == currentDateString) {
            todayMaxTemp = dayObj["wendu1"].toString();
            todayMinTemp = dayObj["wendu2"].toString();
        }
        if (date == tomorrowDateString) {
            tomorrowMaxTemp = dayObj["wendu1"].toString();
            tomorrowMinTemp = dayObj["wendu2"].toString();
        }
        if (date == dayAfterTomorrowDateString) {
            dayAfterTomorrowMaxTemp = dayObj["wendu1"].toString();
            dayAfterTomorrowMinTemp = dayObj["wendu2"].toString();
        }
    }

    // 输出结果
    qDebug() << "Today: Max Temp: " << todayMaxTemp << ", Min Temp: " << todayMinTemp;
    qDebug() << "Tomorrow: Max Temp: " << tomorrowMaxTemp << ", Min Temp: " << tomorrowMinTemp;
    qDebug() << "Day After Tomorrow: Max Temp: " << dayAfterTomorrowMaxTemp << ", Min Temp: " << dayAfterTomorrowMinTemp;
    QString weatherInfo = QString("Today: Max Temp: %1, Min Temp: %2\n"
                                 "Tomorrow: Max Temp: %3, Min Temp: %4\n"
                                 "Day After Tomorrow: Max Temp: %5, Min Temp: %6")
                                 .arg(todayMaxTemp)
                                 .arg(todayMinTemp)
                                 .arg(tomorrowMaxTemp)
                                 .arg(tomorrowMinTemp)
                                 .arg(dayAfterTomorrowMaxTemp)
                                 .arg(dayAfterTomorrowMinTemp);

    // 输出结果到qDebug()
    qDebug() << weatherInfo;
    ui->label_Weather_future->setText(weatherInfo);
}

void MainWindow::disposeWeathertoday(S_HTTP_RESPONE s_back)
{
    QJsonDocument doc = QJsonDocument::fromJson(s_back.Message.toUtf8());

    // 如果解析成功，提取 msg 字段
    if (doc.isObject())
    {
       /* QJsonObject obj = doc.object();
        QString msg = obj.value("msg").toString(); // 提取 msg 字段
        qDebug() << "msg字段的值:" << msg;

        QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());*/
        QJsonObject rootObj = doc.object();

        // 提取所需字段
        double temperature = rootObj["temperature"].toDouble();
        QString place = rootObj["place"].toString();
        QString weather1 = rootObj["weather1"].toString();

        // 输出结果
        qDebug() << "Temperature:" << temperature;
        qDebug() << "Place:" << place;
        qDebug() << "Weather1:" << weather1;

        ui->label_Weather_today->setText("Temperature:" +  QString::number(temperature)+"\n"+"Place:"+ place+"\n"+"Weather:"+ weather1);
    }
    else
    {
        qDebug() << "无效的 JSON 格式";
    }
}

#include "btmanager/bt_test.h"

void MainWindow::displayAudioMeta()
{
    ui->label_aumble->setText(total_info_audio.album);
    ui->label_aumble->setStyleSheet("color:white;");
    //ui->label_duration->setText(convertDurationToTimeFormat(total_info_audio.duration));
    // ui->label_position->setText(convertDurationToTimeFormat(total_info_audio.Position));
    ui->label_title->setText(total_info_audio.title);
    ui->label_title->setStyleSheet("color:white;");
    // qDebug() << "USER PLAY CHANGED !" <<Playing_Album << ": " << p_meta->Album   << "boolean:" <<p_meta->Album.contains(Playing_Album);

    if (Playing_Album != total_info_audio.album)
    {
        // qDebug() << "USER PLAY CHANGED !" << Playing_Album << ": " << total_info_audio.album << "boolean:" << total_info_audio.album.contains(Playing_Album);

        //        if(p_meta->Artist.contains(Playing_Artist) || Playing_Artist =="")
        //        {

        Playing_Artist = total_info_audio.artist;
        Playing_Album = total_info_audio.album;

        qDebug() << "Get picture" << Playing_Artist << "AND " << Playing_Album;
        QString result = Playing_Artist + "+" + Playing_Album;
        GetAlbumPicture(result, "1", "10");
    }

    ui->label_artist->setText(total_info_audio.artist);
    ui->label_artist->setStyleSheet("color:white;");
    //    if(p_meta->Status == "playing" && a_sta != PLAYING)
    //    {
    //        a_sta=PLAYING;
    //    }
    //    else if(p_meta->Status == "pause" && a_sta != PAUSE)
    //    {
    //        a_sta =PAUSE;
    //    }
    //  BTMG_DEBUG("BT playing device: %s", bd_addr);
    qDebug("BT playing music title: %s", total_info_audio.title);
    qDebug("BT playing music artist: %s", total_info_audio.artist);
    qDebug("BT playing music album: %s", total_info_audio.album);
    qDebug("BT playing music track number: %s", total_info_audio.track_num);
    qDebug("BT playing music total number of tracks: %s", total_info_audio.num_tracks);
    qDebug("BT playing music genre: %s", total_info_audio.genre);
    qDebug("BT playing music duration: %s", total_info_audio.duration);
}

void MainWindow::UserAddFinger()
{
    p_finger->AutoEnroll();
}

void MainWindow::displaySpectrum()
{
    if (get_state != BTMG_AVRCP_PLAYSTATE_PLAYING)
    {

        return;
    }

#if 1
    for (int i = 0; i < 30; ++i)
    {
        // 使用 findChild 动态查找每个 QLabel 对象
        QString labelName = QString("label_%1").arg(i); // 动态构造标签名称

        QLabel *a_label = nullptr;
        if (labelName == "label")
            a_label = ui->label;
        // else if (labelName == "label_1") label = ui->label;
        else if (labelName == "label_2")
            a_label = ui->label_2;
        else if (labelName == "label_3")
            a_label = ui->label_3;
        else if (labelName == "label_4")
            a_label = ui->label_4;
        else if (labelName == "label_5")
            a_label = ui->label_5;
        else if (labelName == "label_6")
            a_label = ui->label_6;
        else if (labelName == "label_7")
            a_label = ui->label_7;
        else if (labelName == "label_8")
            a_label = ui->label_8;
        else if (labelName == "label_9")
            a_label = ui->label_9;
        else if (labelName == "label_10")
            a_label = ui->label_10;
        else if (labelName == "label_11")
            a_label = ui->label_11;
        else if (labelName == "label_12")
            a_label = ui->label_12;
        else if (labelName == "label_13")
            a_label = ui->label_13;
        else if (labelName == "label_14")
            a_label = ui->label_14;
        else if (labelName == "label_15")
            a_label = ui->label_15;
        else if (labelName == "label_16")
            a_label = ui->label_16;
        else if (labelName == "label_17")
            a_label = ui->label_17;
        else if (labelName == "label_18")
            a_label = ui->label_18;
        else if (labelName == "label_19")
            a_label = ui->label_19;
        else if (labelName == "label_20")
            a_label = ui->label_20;
        else if (labelName == "label_21")
            a_label = ui->label_21;
        else if (labelName == "label_22")
            a_label = ui->label_22;
        else if (labelName == "label_23")
            a_label = ui->label_23;
        else if (labelName == "label_24")
            a_label = ui->label_24;
        else if (labelName == "label_25")
            a_label = ui->label_25;
        else if (labelName == "label_26")
            a_label = ui->label_26;

        if (a_label)
        { // 如果标签存在
            // 只更改高度，保持宽度不变
            double original_min = 0;      // 原始数据的最小值
            double original_max = 600000; // 原始数据的最大值
            int newHeight = (int)(1 + ((p_thread->spectrumMeta[i] - original_min) / (original_max - original_min)) * (120 - 1));
            if (newHeight < 1)
                newHeight = 1;
            if (newHeight > 120)
                newHeight = 120;

            //  int newHeight = scaled_value;
            //    qDebug() << labelName <<"newHeight"<<newHeight << "meta "<<p_thread->spectrumMeta[i] ;

            a_label->resize(a_label->width(), newHeight);
        }
        else
        {
            //   qDebug() << labelName << "not found!";
        }
    }
    if (switchFlag)
    {

        if (positonoffset == 0 || abs(positonoffset -playing_pos) >5000)
        {
            positonoffset = playing_pos;
        }
        else 
        {
            if (playing_pos-positonoffset>0)
            {
                offsetReduce++;
            }
            else 
            {
                offsetReduce--;
            }
            
        }
        switchFlag = 0;
    }
    positonoffset += offsetReduce;
    currentPosition = static_cast<int>((static_cast<float>(positonoffset) / playing_len) * 800);

    setPlayProgress(currentPosition);

  //  qDebug() << "currentPosition" << currentPosition << "playing_pos" << playing_pos << "playing_len" << playing_len << "get_state" << get_state;

#endif
}
void MainWindow::ElcLockOption()
{
    PULLUP_ELCLOCK;
    //    QTimer *timer = new QTimer();
    //    connect(timer, &QTimer::timeout, this, [](){
    //        qDebug() << "Timeout triggered!";
    //        PULLDOWN_ELCLOCK;
    //    });
    //    timer->start(1000);  // 每20ms更新一次（可根据需要调整旋转速度）
    QTimer::singleShot(1000, this, []()
                       {
                qDebug() << "Timeout triggered!";
                PULLDOWN_ELCLOCK; });

    qDebug() << "TIME START !!!!!!!!!!";
}

void MainWindow::setPlayProgress(int current)
{
    // ui->label_progressbar_point->setGeometry(current,453,20,20);
    ui->label_progressbar_up->setGeometry(0, 460, current, 5);
}

void MainWindow::GetAlbumPicture(QString Artist, QString page, QString limit)
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
    query.addQueryItem("page", page);
    query.addQueryItem("limit", limit);
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
    else if (s_back.Title == HOTSEARCH)
    {
        DisposeHotSearch(s_back);
    }
    else if (s_back.Title == DATETODAY)
    {
        DisposeDate(s_back);
    }
    else if (s_back.Title == WEATHERTODAY)
    {
        //DisposeDate(s_back);
        disposeWeathertoday(s_back);
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

    qDebug() << "udhcpc result : ";
    p_http->sendGetRequest(QUrl("http://img0.baidu.com/it/u=4101285560,1286785277&fm=253&fmt=auto&app=120&f=JPEG"));
}

void MainWindow::flushNetUI()
{
    qDebug() << "VAEVAEAVEVAEVAEVAVE";

    GetDeviceIP();
    GetHotSearch();
    GetDateToday();
    GetWeatherToday();
    //GetOnewords();
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
    system("wifi -c ThanksGivingDay_111 Zz123456\n"); // todo
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
       // ui->label_soulwords->setText(msg);
    }
    else
    {
        qDebug() << "无效的 JSON 格式";
    }
}
void MainWindow::DisposeDate(S_HTTP_RESPONE s_back)
{
    QJsonDocument doc = QJsonDocument::fromJson(s_back.Message.toUtf8());

    // 如果解析成功，提取 msg 字段
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        QString msg = obj.value("msg").toString(); // 提取 msg 字段
        qDebug() << "date msg字段的值:" << msg;

        qint64 timestamp = msg.toLongLong();

            // 将时间戳转换为 QDateTime 对象
            dateTime = QDateTime::fromTime_t(timestamp);

            // 获取年月日，存入数组
            int year = dateTime.date().year();
            int month = dateTime.date().month();
            int day = dateTime.date().day();
            int hour = dateTime.time().hour();
            int minute = dateTime.time().minute();
            int second = dateTime.time().second();

            // 输出年月日为数组形式
            QString dateFormatted = QString("%1/%2/%3").arg(year).arg(month).arg(day);
            // 输出时分秒为 "hh:mm:ss" 的格式
            QString timeFormatted = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));

            // 打印结果
            qDebug() << "Formatted Date:" << dateFormatted;
            qDebug() << "Formatted Time:" << timeFormatted;

        ui->label_date->clear();
        ui->label_date->setText(timeFormatted+"\n\n"+dateFormatted);
    }
    else
    {
        qDebug() << "无效的 JSON 格式";
    }
}
void MainWindow::DisposeHotSearch(S_HTTP_RESPONE s_back)
{
    QJsonDocument doc = QJsonDocument::fromJson(s_back.Message.toUtf8());

    // 如果解析成功，提取 msg 字段
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        QStringList titles;

        // Extract the "data" array from the JSON object
        QJsonArray dataArray = obj["data"].toArray();

        // Iterate over the array and extract each title
        for (const QJsonValue &value : dataArray) {
            QJsonObject item = value.toObject();
            QString title = item["query"].toString();
            titles.append(title);
        }

        // Output the titles for debugging purposes
        qDebug() << titles;

        ui->label_HotSearch->setText(titles.at(0));
    }
    else
    {
        qDebug() << "无效的 JSON 格式";
    }
}
int MainWindow::DisposePciteureJson(S_HTTP_RESPONE s_back)
{
    qDebug() << s_back.Message;
    static int errcnt = 0;
    QString jsonString = s_back.Message;

    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject())
    {
        qDebug() << "Invalid JSON format!" << jsonString;
        return -1;
    }

    // 获取根对象
    QJsonObject jsonObj = doc.object();
    //   QStringList getUrl;
    // 判断 code 是否为 200
    if (jsonObj.value("code").toInt() == 200)
    {
        QJsonArray resArray = jsonObj.value("res").toArray();

        // 提取前10个URL
        int count = 0;
        for (const QJsonValue &value : resArray)
        {

            qDebug() << value.toString();
            getUrl.append(value.toString());
            count++;
        }
        picGetcnt = count;
        DownloadAudioPctrue(getUrl.at(picSearchCnt++));
    }
    else
    {

        qDebug() << "Code is not 200";
        if (doc.object().value("msg").toString().contains("参数过长"))
        {

            QString fixKeyword;
            if (Playing_Album.length() <= Playing_Artist.length())

                fixKeyword = Playing_Album;

            else

                fixKeyword = Playing_Artist;

            if (fixKeyword.length() > 24)
            {

                fixKeyword = fixKeyword.left(24);
            }

            QRegExp regex("[^A-Za-z0-9]");
            fixKeyword = fixKeyword.replace(regex, "");
            qDebug() << "参数过长 校正后" << fixKeyword;
            if (errcnt++ >= 10)
            {
            }
            else
            {
                badKeywords = true;
                GetAlbumPicture(fixKeyword, "1", "10");
            }
            sleep(1);
            return -1;
        }
        else if (doc.object().value("msg").toString().contains("Syntax error"))
        {
            static bool isAlbum = false;
            QRegExp regex("[^A-Za-z0-9]");
            QString fixKeyword;
            if (isAlbum)
                fixKeyword = Playing_Album.replace(regex, "");
            else
            {
                fixKeyword = Playing_Artist.replace(regex, "");
            }
            if (fixKeyword.length() > 24)
            {

                fixKeyword = fixKeyword.left(24);
            }
            isAlbum = true;
            GetAlbumPicture(fixKeyword, "1", "1");
        }
        else if (doc.object().value("msg").toString().contains("Control character error"))
        {
            static bool isAlbum = false;
            QRegExp regex("[^A-Za-z0-9]");
            QString fixKeyword;
            if (isAlbum)
                fixKeyword = Playing_Album.replace(regex, "");
            else
            {
                fixKeyword = Playing_Artist.replace(regex, "");
            }
            if (fixKeyword.length() > 24)
            {

                fixKeyword = fixKeyword.left(24);
            }
            isAlbum = true;
            GetAlbumPicture(fixKeyword, "1", "1");
        }
        else /*if(doc.object().value("msg").toString().contains("请求失败，请重试！"))*/
        {
            GetAlbumPicture(Playing_Album, "1", "10");
        }
        sleep(1);
        return -1;
    }
}
/*            if (badKeywords)
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
            GetAlbumPicture(Playing_Album+" 封面");*/
void MainWindow::displayAlbumPicOnlabel(QByteArray bytes)
{
    QByteArray imageData = bytes;

    // 将下载的图片数据转换成 QImage
    QImage image;

    if (image.loadFromData(imageData))
    {
        if (isImageWhite(image, 200))
        {
            qDebug() << "this picture is not good ,try to refind one  ";
            qDebug() << "current count :" << picSearchCnt << "total source: " << picGetcnt;
            if (picSearchCnt < picGetcnt)
            {
                DownloadAudioPctrue(getUrl.at(picSearchCnt++));
                return;
            }
        }

        // 将 QImage 转换为 QPixmap

        

#ifdef RotationLabel
        QPixmap pixmap = QPixmap::fromImage(image);
        label->move(530, 40); // 设置标签位置

        label->loadImage(pixmap); // 设置标签的图片
        label->show();
        
#endif
  //  QImage *img = new QImage;                       //新建一个image对象
   // img->load(":/picture/faild.png");               //将图像资源载入对象img，注意路径，可点进图片右键复制路径
    ui->label_aumblePic->setPixmap(QPixmap::fromImage(image)); //将图片放入label，使用setPixmap,注意指针*img
    ui->label_aumblePic->setAlignment(Qt::AlignCenter);
    ui->label_aumblePic->setScaledContents(true);
        getUrl.clear();
        picSearchCnt = 0;
        picGetcnt = 0;
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
