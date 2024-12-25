#include "audio_info.h"
#include "QProcess"
#include "QDebug"



audio_info::audio_info(QString Hwid)
{
    devid="dev_"+ Hwid.replace(":", "_");
    qDebug() <<"Get devid :" <<devid;
}

void audio_info::run()
{
    while(1)
    {
        usleep(1000*500);//0.5s fresh

        GetAudioInfo();
        DisposeTitleFile();
        emit  updateMeta(&meta_data);
    }
}

void audio_info::GetAudioInfo()
{
    char gettitle[2048]={0};
    char*  ch;
    QByteArray ba = devid.toLatin1(); // must
    ch=ba.data();
    /*sprintf(gettitle,"dbus-send --system --type=method_call --print-reply --dest=org.bluez /org/bluez/hci0/%s/player0 org.freedesktop.DBus.Properties.Get string:org.bluez.MediaPlayer1 string:Track > %s",\
            ch,TITLE_PATH);*/
    sprintf(gettitle,"dbus-send --system --print-reply --type=method_call --dest=org.bluez / org.freedesktop.DBus.ObjectManager.GetManagedObjects > %s",TITLE_PATH);
    //qDebug() << "Send command:"<<gettitle;
    system(gettitle);


    //char*  ch;
    /* ba = devid.toLatin1(); // must
    ch=ba.data();
    sprintf(gettitle,"dbus-send --system --type=method_call --print-reply --dest=org.bluez /org/bluez/hci0/%s/player0 org.freedesktop.DBus.Properties.Get string:org.bluez.MediaPlayer1 string:Position > %s",\
            ch,POSITION_PATH);
   // qDebug() << "Send command:"<<gettitle;
    system(gettitle);*/

}
void audio_info::DisposeTitleFile()
{

    QString content2 = readFileContent(TITLE_PATH);  // 获取文件内容

          if (!content2.isEmpty()) {
            //  qDebug() << "文件内容：" << content2;
              extractFields(content2);
          }
        //  meta_data.print();
}



QString  audio_info::readFileContent(const QString &filePath) {
    QFile file(filePath);  // 打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filePath;
        return QString();  // 打开文件失败，返回空字符串
    }

    QTextStream in(&file);  // 创建文本流对象
    QString content = in.readAll();  // 读取整个文件的内容到QString

    file.close();  // 关闭文件
    return content;  // 返回文件内容
}
int audio_info::QprocessCommand(QString program, QString &output, QStringList arguments)
{

     QProcess process;
    // 启动外部进程
    process.start(program, arguments);

    // 等待进程执行完毕
    if (!process.waitForStarted()) {
        qCritical() << "Failed to start the process";
        return -1;
    }

    // 等待进程执行完成
    if (!process.waitForFinished()) {
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


void audio_info::extractFields(const QString &input) {
    // 定义正则表达式模式，用于匹配键值对
  QRegExp regex("string\\s*\"([^\"]+)\"\\s*variant\\s*\\w+\\s*(\"[^\"]+\"|\\d+)");
    // 存储提取的结果
    QString title, album, artist,duration,position,status;
    //uint32_t  duration=0;

    // 正则表达式匹配
    int pos = 0;
    while ((pos = regex.indexIn(input, pos)) != -1) {
        QString key = regex.cap(1);   // 获取键
        QString value = regex.cap(2); // 获取值
        if (value.startsWith("\"") && value.endsWith("\"")) {
            value = value.mid(1, value.length() - 2);
        }
        // 判断不同的键，提取相应的值
        if (key == "Title") {
            title = value;
        } else if (key == "Album") {
            album = value;
        } else if (key == "Artist") {
            artist = value;
        } else if (key == "Duration") {
            // 解析 Duration，假设是整数
          duration = value;
        }
        else if (key == "Position") {
                    // 解析 Duration，假设是整数
                  position = value;
                }
        else if (key == "Status") {
                    // 解析 Duration，假设是整数
                  status = value;
                }
        pos += regex.matchedLength(); // 移动位置
    }

    // 输出结果
   /* qDebug() << "Title:" << title;
    qDebug() << "Album:" << album;
    qDebug() << "Artist:" << artist;
    qDebug() << "Duration:" << duration;*/

    meta_data.Album =album;
    meta_data.Title =title;
    meta_data.Duration =duration;
    meta_data.Artist =artist;
    meta_data.Position =position;
    meta_data.Status =status;

}
#if 0
//QprocessBlueToothCtl();
QString res;
QStringList arguments;

arguments.clear();
arguments << "--system";
arguments << "--type=method_call";
arguments << "--print-reply";
arguments << "--dest=org.bluez";
QString tmpdevid =devid;
QString path ="/org/bluez/hci0/" + tmpdevid;
arguments << path;
arguments << "/player0 org.freedesktop.DBus.Properties.Get string:org.bluez.MediaPlayer1" ;
arguments << "string:Track";
QprocessCommand("dbus-send",res,arguments);
qDebug() << res;
#endif
