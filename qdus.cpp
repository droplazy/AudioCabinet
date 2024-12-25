#include "qdus.h"
#include "audio_info.h"
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QtDBus>
#include <QRegularExpression>
#include "mainwindow.h"


typedef QMap<QString, QMap<QString, QVariant> > ConnectionDetails;
Q_DECLARE_METATYPE(ConnectionDetails)

qDbus::qDbus()
{
    //QprocessBlueToothCtl();
   QString res;
    QStringList arguments;
    arguments.clear();
    arguments << "power";
    arguments << "on";
    QprocessCommand("bluetoothctl",res,arguments);
    qDebug() << "bluetoothctl power on  : " <<res ;

    arguments.clear();
    arguments << "agent";
    arguments << "on";
    QprocessCommand("bluetoothctl",res,arguments);
    qDebug() << "bluetoothctl agent  on: " <<res;

    arguments.clear();
    arguments << "default-agent";
    QprocessCommand("bluetoothctl",res,arguments);
    qDebug() << "bluetoothctl default-agent : " <<res;

    arguments.clear();
    arguments << "info";
    QprocessCommand("bluetoothctl",res,arguments);
    //qDebug() << "bluetoothctl info : " <<res;
    if(res.contains("Missing device address argument"))
    {
        qDebug() << "No BL devices Connect";
    }
    else
    {
        CheckBlueDevice(res);
    }

//    arguments.clear();
//    arguments << "scan";
//    arguments << "on";
//    //arguments << "&";

//    QprocessCommand("bluetoothctl",res,arguments);
    system("bluetoothctl scan on & \r\n");
    qDebug() << "bluetoothctl scan on! : " <<res;

}

void qDbus::run()
{
    qDBusRegisterMetaType<ConnectionDetails>();
    while(1)
    {
        sleep(1);

        if(!isBlconnected)
        {
        TrustScanDevices();
        CheckTrustDevicesList();
        }

        QString rel =GetPairingStatus();
        if(rel.contains("Device "))
        {
            isBlconnected=true;
            CheckBlueDevice(rel);
            if(!isblueAlsaTurnon)// A4:F6:E8:06:B2:1F --pcm=hw:0,0
            {
//                QString res;
//                 QStringList arguments;
//                 arguments<<HwId;
//                 arguments<<"--pcm=hw:0,0";
//                 QprocessCommand("bluealsa-aplay",res,arguments);
//                 qDebug() << "bluealsa  aplay res : " <<res;

                char cmd[256]={0};
                char*  ch;
                QByteArray ba = HwId.toLatin1(); // must
                ch=ba.data();
                sprintf(cmd,"bluealsa-aplay %s --pcm=hw:0,0 &",ch);//2C:8D:B1:66:8F:81
                system(cmd);
                qDebug() << cmd;

                isblueAlsaTurnon =true;
            }
        }
        else
        {
            isBlconnected=false;
            isblueAlsaTurnon = false ;
            if(isblueAlsaTurnon)
            system("ps -ef | grep bluealsa-aplay | grep -v grep | xargs kill");
        }

         if(isblueAlsaTurnon && !isMETAON)
         {
            p_audio = new audio_info(HwId);
            p_audio->start();
            isMETAON= true;
            qDebug()<< "Meta Audio  is ON";
            connect(p_audio, SIGNAL(updateMeta(S_Aduio_Meta *)), p_main, SLOT(displayAudioMeta(S_Aduio_Meta *)), Qt::AutoConnection);						 // updatefile
           // emit updateAudiostaus(PLAYING);
         }
    }
}

QString qDbus::GetPairingStatus()// use qdbus has some issue
{
    QString res;
     QStringList arguments;
     arguments << "info";
     QprocessCommand("bluetoothctl",res,arguments);
     //qDebug() << "cmd res : " <<res;
    return res;
}
bool qDbus::VolumeDown()
{
    // 创建 QDBusInterface 对象，连接到 BlueZ 服务
    QDBusInterface interface("org.bluez",  // 蓝牙服务名
                            "/org/bluez/hci0/dev_A4_F6_E8_06_B2_1F", // 蓝牙设备对象路径
                            "org.bluez.MediaControl1", // 接口名
                            QDBusConnection::systemBus()); // 使用系统总线

    // 调用 VolumeDown 方法
    QDBusReply<void> reply = interface.call("VolumeDown");

    // 检查调用结果
    if (reply.isValid()) {
        qDebug() << "VolumeDown method called successfully.";
    } else {
        qDebug() << "Failed to call VolumeDown method:" << reply.error().message();
    }

}

int qDbus::QprocessCommand(QString program, QString &output, QStringList arguments)
{
    // 设置启动的外部程序（比如运行 shell 命令 `echo`）
    //QString program = "echo";
//    QStringList arguments;
//    arguments << "Hello, QProcess!";

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

void qDbus::QprocessBlueToothCtl()
{

}

void qDbus::CheckTrustDevicesList()
{
    QString res;
    QStringList arguments;
    for (const QString &serial : Trsutlist)
    {
       // qDebug() << serial;
        arguments.clear();
        arguments << "info";
        arguments << serial;
        QprocessCommand("bluetoothctl",res,arguments);
       // qDebug() << "bluetoothctl info: " <<res;

            if(res.contains("Trusted: no"))
            {
               // qDebug() << serial;
                arguments.clear();
                arguments << "trust";
                arguments << serial;
                QprocessCommand("bluetoothctl",res,arguments);
               // qDebug() << "bluetoothctl trust: " <<res;

                    if(res.contains("trust succeeded"))
                    {
                        Trsutlist.append(serial);
                    }
                    else
                    {
                         Trsutlist.removeOne(serial);
                    }
            }

    }
}

void qDbus::TrustScanDevices()
{
    QString res;
    QStringList arguments;
    //QStringList devices;
    arguments.clear();
    arguments << "devices";
    QprocessCommand("bluetoothctl",res,arguments);
   // qDebug() << "bluetoothctl devices: " <<res;

    QRegularExpression regex(R"(\s([A-F0-9:]{17})\s)");

    // 使用正则表达式匹配所有序列号
    QRegularExpressionMatchIterator iterator = regex.globalMatch(res);

    QStringList deviceSerialNumbers;

    // 提取所有的设备序列号
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        // 提取并存储序列号
        QString serialNumber = match.captured(1); // 捕获正则中的第一个匹配项
        deviceSerialNumbers.append(serialNumber);
    }

    // 输出结果
    //qDebug() << "Extracted device serial numbers:";
    for (const QString &serial : deviceSerialNumbers) {
        if(!Trsutlist.contains(serial))
        {
       // qDebug() << serial;
        arguments.clear();
        arguments << "trust";
        arguments << serial;
        QprocessCommand("bluetoothctl",res,arguments);
      //  qDebug() << "bluetoothctl trust: " <<res;

            if(res.contains("trust succeeded"))
            {
                Trsutlist.append(serial);
            }
        }
    }


}

bool qDbus::CheckBlueDevice(QString BlueInfo)
{
    QString getData;
    int postion =0;
    /******************/
    postion = BlueInfo.indexOf("Device ");
    if (postion != -1) {
        QString subText = BlueInfo.mid(postion);
        getData =subText.mid(7,17);
    //    qDebug()<<"DEVICES:"<<getData;
        HwId  =getData;
    } else {
        qDebug() << "Keyword not found.";
    }
    /***********************/
    postion = BlueInfo.indexOf("Name: ");
    if (postion != -1) {
        QString subText = BlueInfo.mid(postion);
        getData =subText.mid(6,32);
         postion = getData.indexOf("\n");
         getData = getData.left(postion);
         //getData.at(postion)= '\0';
     //   qDebug()<<"Name:"<<getData ;
        BLname =getData ;
    } else {
        qDebug() << "Keyword not found.";
    }
    /**************************/
    postion = BlueInfo.indexOf("Alias: ");
    if (postion != -1) {
        QString subText = BlueInfo.mid(postion);
        getData =subText.mid(7,32);
         postion = getData.indexOf("\n");
         getData = getData.left(postion);
         //getData.at(postion)= '\0';
     //   qDebug()<<"Alias:"<<getData ;
        BLname =Alias ;
    } else {
        qDebug() << "Keyword not found.";
    }
    /**************************/
    postion = BlueInfo.indexOf("Paired: ");
    if (postion != -1) {
        QString subText = BlueInfo.mid(postion);
        getData =subText.mid(8,5);
         postion = getData.indexOf("\n");
         getData = getData.left(postion);
         //getData.at(postion)= '\0';
      //  qDebug()<<"Paired:"<<getData ;
        if(getData.contains("no"))
        {
      //      qDebug() << "Paired devives   HW id :" << HwId;

        }
        else if(getData.contains("yes"))
        {
      //       qDebug() << "Paired devives   HW id :" << HwId;
        }
        else
        {
            qDebug() << "UNKNOW ERROR: no result find  HW id :" << HwId;
        }
    } else {
        qDebug() << "Keyword not found.";
    }

}


/*
        usleep(1000*1000);
        GetPairingStatus();
        if (bluetoothProcess.waitForReadyRead()) {
            QString output = bluetoothProcess.readAll();
            qDebug() << "bluetoothctl output:" << output << "\n";
            if(output.contains("Device "))
            {
                    int postion =output.indexOf("Device ");
                    if (postion != -1)
                    {
                        QString subText = output.mid(postion);
                        QString getData =subText.mid(7,17);

                        postion = getData.indexOf("\n");
                        getData = getData.left(postion);
                        //getData.at(postion)= '\0';

                        getData = "trust "+getData;
                        qDebug()<<"DEVICES:"<<getData;
                        bluetoothProcess.write("trust  \n");
                        bluetoothProcess.waitForBytesWritten();
                    }
            }
            else if (output.contains("Confirm passkey"))
            {
                // 发送命令到 bluetoothctl
                bluetoothProcess.write("yes\n");
                bluetoothProcess.waitForBytesWritten();
            }
        }
*/
























