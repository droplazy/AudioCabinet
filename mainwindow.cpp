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
    //setPlayProgress(0);
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::displaySpectrum);
    timer->start(100); // 每20ms更新一次（可根据需要调整旋转速度）

    QTimer *timer_2 = new QTimer();
    connect(timer_2, &QTimer::timeout, this, &MainWindow::displaySpectrumFall);
    timer_2->start(1); // 每20ms更新一次（可根据需要调整旋转速度）
    CreatSpectrum();
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

void MainWindow::initSepctrum()
{
    for (int i = 1; i <= 72; ++i)
    {
        // 使用 findChild 动态查找每个 QLabel 对象
        QString labelName = QString("label_spectrum_%1").arg(i); // 动态构造标签名称
        QLabel *a_label = this->findChild<QLabel *>(labelName);

        if (a_label)
        { // 如果标签存在

            QPoint currentPos = a_label->pos();

            a_label->setGeometry(currentPos.x(),430,14,120);
        }
        else
        {
               qDebug() << labelName << "not found!";
        }
    }
}
// 设置 QLabel 的字体和大小（可选）
//   label->setStyleSheet("font-size: 20px; font-weight: bold; color: blue;");
void MainWindow::CreatSpectrum()
{
    int spacing = 2;
    int x = 3;
    int total_labels = 60;  // 假设有61个标签
    int center_index = total_labels / 2;  // 计算中间标签的索引

    QImage *img_1 = new QImage;  // 新建一个image对象
    img_1->load(":/Spectrum_single.png");  // 将图像资源载入对象img，注意路径
    QImage *img_2 = new QImage;  // 新建一个image对象
    img_2->load(":/Spectrum_single2.png");  // 将图像资源载入对象img，注意路径

    // 计算标签的位置：从中间开始依次向两边散开
    for (int i = 0; i < total_labels; i++) {
        int offset = i / 2;  // 偏移量，用于计算标签的位置
        int label_position;

        // 如果是偶数索引，表示从中间往左放
        if (i % 2 == 0) {
            label_position = center_index - offset;
        } else {  // 如果是奇数索引，表示从中间往右放
            label_position = center_index + offset + 1;  // 在奇数位置加一个额外的偏移量，防止重叠
        }

        // 创建并设置 labels_bottom
        labels_bottom[i] = new QLabel(this);
        labels_bottom[i]->setObjectName(QString("spectrum_bottom_%1").arg(i + 1));  // 显式设置 objectName
        labels_bottom[i]->move(x + label_position * (11 + spacing), 480 - 3);  // 设置 QLabel 的位置
        labels_bottom[i]->setPixmap(QPixmap::fromImage(*img_2));
        labels_bottom[i]->resize(11, 111);

        // 创建并设置 labels_top
        labels_top[i] = new QLabel(this);
        labels_top[i]->setObjectName(QString("spectrum_top_%1").arg(i + 1));  // 显式设置 objectName
        labels_top[i]->move(x + label_position * (11 + spacing), 480);  // 设置 QLabel 的位置
        labels_top[i]->setPixmap(QPixmap::fromImage(*img_1));
        labels_top[i]->setStyleSheet("font-size: 14px; font-weight: bold; color: red;");  // 可选样式
        labels_top[i]->raise();  // 将 labels_top 放在最上层
        labels_top[i]->resize(11, 92);

        //qDebug() << i << labels_top[i]->objectName() << "X:" << labels_top[i]->pos().x();
    }

    // 创建进度条标签
    /*label_progressbar_bottom = new QLabel(QString("label_progressbar_bottom"), this);
    label_progressbar_top = new QLabel(QString("label_progressbar_top"), this);

    label_progressbar_top->resize(800, 5);
    label_progressbar_bottom->resize(800, 5);

    label_progressbar_top->clear();
    label_progressbar_bottom->clear();

    label_progressbar_top->setStyleSheet("background-color: rgb(46, 87, 47);border-radius:2px;");  // 可选样式
    label_progressbar_bottom->setStyleSheet("background-color: rgb(136, 138, 133);");  // 可选样式

    label_progressbar_bottom->move(0, 200);
    label_progressbar_bottom->raise();
    label_progressbar_top->move(0, 200);
    label_progressbar_top->raise();*/
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
void MainWindow::smoothData(double spectrumMeta[], int length, double smoothingFactor) {
    // 使用加权移动平均来平滑数据
    // smoothingFactor 表示平滑程度，取值范围 0 到 1，越接近 1 越平滑

    const double MAX_VALUE = 99999999.0; // 设置最大值

    std::vector<double> smoothedData(length);

    // 第一个元素的平滑值就是它自己
    smoothedData[0] = spectrumMeta[0];

    // 对剩余元素进行平滑处理
    for (int i = 1; i < length; i++) {
        smoothedData[i] = smoothingFactor * spectrumMeta[i] + (1 - smoothingFactor) * smoothedData[i - 1];
    }

    // 找到平滑数据中的最大值
    double maxSmoothedValue = *std::max_element(smoothedData.begin(), smoothedData.end());

    // 如果最大值超过了设定的最大值，则进行缩放
    if (maxSmoothedValue > MAX_VALUE) {
        double scaleFactor = MAX_VALUE / maxSmoothedValue;  // 计算缩放比例

        // 缩放所有平滑后的数据
        for (int i = 0; i < length; i++) {
            smoothedData[i] *= scaleFactor;
        }
    }

    // 将平滑后的数据重新赋值给原数组
    for (int i = 0; i < length; i++) {
        spectrumMeta[i] = smoothedData[i];
    }
}


void MainWindow::UserAddFinger()
{
    p_finger->AutoEnroll();
}
/*
// 使用 findChild 动态查找每个 QLabel 对象
QString labelName = QString("spectrum_bottom_%1").arg(i); // 动态构造标签名称
QLabel *a_label = this->findChild<QLabel *>(labelName);

QLabel *a_label = nullptr;
if (labelName == "label_spectrum_1")
    a_label = ui->label_spectrum_1;
else if (labelName == "label_spectrum_2")
    a_label = ui->label_spectrum_2;
*/
void MainWindow::updateDisplayTime()
{
    // 获取当前系统时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
   // qDebug() << "当前系统时间:" << currentDateTime.toString("yyyy-MM-dd HH:mm:ss");

    // 使用 currentDateTime 获取毫秒级时间戳
    qint64 timestamp = currentDateTime.toMSecsSinceEpoch();

    // 将时间戳转换为 QDateTime 对象
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);

    // 获取年月日，存入数组
    int year = dateTime.date().year();
    int month = dateTime.date().month();
    int day = dateTime.date().day();
    int hour = dateTime.time().hour();
    int minute = dateTime.time().minute();
    int second = dateTime.time().second();

    // 输出年月日为数组形式
    QString dateFormatted = QString("%1-%2-%3").arg(year).arg(month).arg(day);
    // 输出时分秒为 "hh:mm:ss" 的格式
    QString timeFormatted = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));

    // 打印结果
    // qDebug() << "Formatted Date:" << dateFormatted;
    // qDebug() << "Formatted Time:" << timeFormatted;

    // 设置文本到 UI
    ui->label_date->clear();
    ui->label_date->setText(timeFormatted + "\n\n" + dateFormatted);
}
void MainWindow::displaySpectrumFall()
{   
     if (get_state != BTMG_AVRCP_PLAYSTATE_PLAYING)
    {

       // qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        return;
    }
     for (int i = 1; i <= 60; ++i)
    {

        if (labels_top[i-1] && labels_bottom[i-1])
        { // 如果标签存在
        QPoint currentPos = labels_bottom[i-1]->pos();
        int distance = labels_top[i-1]->pos().y() - currentPos.y();
        
        // 确保distance值在 [2, 92] 范围内
        if (distance < 2) distance = 2;
        if (distance > 92) distance = 92;
        
        // 根据distance计算fallspeed值，映射到5到1的范围，并强制转换为整数
         fallspeed[i-1] = static_cast<int>(3 - (float)(distance - 2) / (92 - 2) * (3 - 1) + 0.5f); // 使用+0.5进行四舍五入
        
        // 在这里使用fallspeed进行相关操作
      //  qDebug() <<i<< "=Distance:" << distance << " Fallspeed:" << fallspeed[i-1] <<" FallCount:" << fallCount[i-1];
                if(currentPos.y() <480-3)
                {

              //      fallspeed[i-1]  =fallspeed[i-1] *2;
                    if((fallCount[i-1] ++ )  >= fallspeed[i-1])
                    {
                        currentPos.setY(currentPos.y()+1);  // 使用 setY() 来修改 y 坐标
                        fallCount[i-1] =0;
                    }
                    
                }
            labels_bottom[i-1]->move(currentPos.x(),currentPos.y());

        }
        else
        {
               qDebug() << i<< "not found!";
        }
    }


}

void MainWindow::displaySpectrum()
{
   // static int fall = 0;
    if (get_state != BTMG_AVRCP_PLAYSTATE_PLAYING)
    {

       // qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        return;
    }
    double original_min = 0;      // 原始数据的最小值
    double original_max = 99999999; // 原始数据的最大值


    smoothData(p_thread->spectrumMeta,60,0.5);
  /*  for (int i = 0; i <60; ++i)
    {
        double currentValue = p_thread->spectrumMeta[i - 1];  // 获取当前值

                // 更新最大值
                if (currentValue > original_max) {
                    original_max = currentValue;
                }

                // 更新最小值
                if (currentValue < original_min) {
                    original_min = currentValue;
                }
    }
        original_max = (original_max < 5658100) ? 5658100 : (original_max < 56581000 ? 56581000 : original_max);
        qDebug() << "original_min" <<original_min <<"original_max" <<original_max;*/
#if 1
    for (int i = 1; i <= 60; ++i)
    {

        if (labels_top[i-1])
        { // 如果标签存在
            // 只更改高度，保持宽度不变
            int newHeight = (int)(1 + ((p_thread->spectrumMeta[i-1] - original_min) / (original_max - original_min)) * (92  - 1));
            if (newHeight < 1)
                newHeight = 1;
            if (newHeight > 92)
                newHeight = 92;

            // qDebug() << i << labels_top[i-1]->objectName();
            //  int newHeight = scaled_value;
            //    qDebug() << labelName <<"newHeight"<<newHeight << "meta "<<p_thread->spectrumMeta[i] ;

           // a_label->resize(a_label->width(), newHeight); 370 ~430 60
            QPoint currentPos = labels_top[i-1]->pos();

            labels_top[i-1]->move(currentPos.x(),480-newHeight);

            currentPos =labels_bottom[i-1]->pos();
            if(labels_top[i-1]->pos().y() <= currentPos.y())
            {
                    currentPos.setY(labels_top[i-1]->pos().y() - 3);  // 使用 setY() 来修改 y 坐标
            }
           //                 fallspeed[i-1] =(labels_top[i-1]->pos().y() - currentPos.y()  ) ;

          /*  else
            {
                fallspeed[i-1] =(labels_top[i-1]->pos().y() - currentPos.y()  ) ;
                qDebug() <<"fallspeed[" << i-1 <<"] = " <<fallspeed[i-1]  ;

                if(currentPos.y() <480-3)
                {

                    //if(currentPos.y()-labels_top[i-1]->pos().y()  >=3)
                    if((fallspeed[i-1] --) > =2  )
                    {
                    currentPos.setY(currentPos.y()+1);  // 使用 setY() 来修改 y 坐标

                    }
                    
                }
            }*/

        //    currentPos.setY(currentPos.y()-1);  // 使用 setY() 来修改 y 坐标
            labels_bottom[i-1]->move(currentPos.x(),currentPos.y());
//            qDebug() << i << labels_top[i-1]->objectName() << "Y:" << labels_top[i-1]->pos().y();
//            qDebug() << i << labels_bottom[i-1]->objectName() << "Y:" << labels_bottom[i-1]->pos().y();

        }
        else
        {
               qDebug() << i<< "not found!";
        }
    }
   /* if (switchFlag)
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
    currentPosition = static_cast<int>((static_cast<float>(positonoffset) / playing_len) * 800);*/
   // setPlayProgress(currentPosition);

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
    //label_progressbar_top->setGeometry(0, 460, current, 5);
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
/*
void MainWindow::wifiLaunch()
{
    // 检查 wifi_daemon 是否已经运行
    QProcess process;
    process.start("pgrep", QStringList() << "wifi_daemon");
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();
    if (output.isEmpty()) {
        // 如果没有找到运行中的 wifi_daemon，则启动它
        qDebug() << "Starting wifi_daemon...";
        process.start("wifi_daemon");
        if (!process.waitForStarted()) {
            qDebug() << "Failed to start wifi_daemon.";
            return;
        }
        process.waitForFinished();
        qDebug() << "wifi_daemon started.";
    } else {
        qDebug() << "wifi_daemon is already running.";
    }

    // 配置 wifi 为 STA 模式
    process.start("wifi", QStringList() << "-o" << "sta");
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start wifi in STA mode.";
        return;
    }
    process.waitForFinished();
    qDebug() << "wifi set to STA mode.";

    // 连接到指定的 Wi-Fi 网络
    process.start("wifi", QStringList() << "-c" << "ThanksGivingDay_111" << "Zz123456");
    if (!process.waitForStarted()) {
        qDebug() << "Failed to connect to wifi network.";
        return;
    }
    process.waitForFinished();
    qDebug() << "Connected to wifi network 'ThanksGivingDay_111'.";
}*/


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
        QString dateFormatted = QString("%1-%2-%3").arg(year).arg(month).arg(day);
        // 输出时分秒为 "hh:mm:ss" 的格式
        QString timeFormatted = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));

        // 打印结果
        qDebug() << "Formatted Date:" << dateFormatted;
        qDebug() << "Formatted Time:" << timeFormatted;

        // 设置文本到 UI
        ui->label_date->clear();
        ui->label_date->setText(timeFormatted + "\n\n" + dateFormatted);

        // 组合成 "YYYY-MM-DD HH:MM:SS" 格式
        QString dateTimeStr = QString("%1 %2").arg(dateFormatted).arg(timeFormatted);

        // 使用 QProcess 调用系统的 `date` 命令设置时间
        QProcess *process = new QProcess(this);
        QString command = QString("date -s \"%1\"").arg(dateTimeStr);

        // 执行命令
        process->start(command);
        if (!process->waitForStarted()) {
            qDebug() << "Failed to start process.";
        } else {
            process->waitForFinished();
            qDebug() << "Date set successfully.";
        }

        // 通过硬件时钟同步系统时间
        process->start("sudo hwclock --systohc");
        process->waitForFinished();
        qDebug() << "Hardware clock synced with system time.";

        QTimer *timer = new QTimer();
        /* connect(timer, &QTimer::timeout, this, [](){
             qDebug() << "Timeout aaatriggered!";
           //  PULLDOWN_ELCLOCK;
         });*/
        connect(timer, &QTimer::timeout, this, &MainWindow::updateDisplayTime);
        timer->start(500); // 每20ms更新一次（可根据需要调整旋转速度）
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

            QRegExp regex("[^A-Za-z0-9\\u4e00-\\u9fa5]");
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
            QRegExp regex("[^A-Za-z0-9\\u4e00-\\u9fa5]");
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
       /*     static bool isAlbum = false;
            QRegExp regex("[^A-Za-z0-9\\u4e00-\\u9fa5]");
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
            isAlbum = true;*/
            QString result = Playing_Artist + "+" + Playing_Album;
           // GetAlbumPicture(result, "1", "10");
            GetAlbumPicture(result, "1", "1");
        }
        else /*if(doc.object().value("msg").toString().contains("请求失败，请重试！"))*/
        {
            GetAlbumPicture(Playing_Album, "1", "10");
        }
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
