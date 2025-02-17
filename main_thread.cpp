#include "main_thread.h"
#include "btmanager/bt_test.h"


main_thread::main_thread()
{
    manager = new QNetworkConfigurationManager(this);

    // 当网络配置改变时进行响应
    connect(manager, &QNetworkConfigurationManager::configurationChanged, this, &main_thread::checkNetworkStatus);
    connect(manager, &QNetworkConfigurationManager::onlineStateChanged, this, &main_thread::checkNetworkStatus);

    // 初始检查网络状态
    checkNetworkStatus();
}

void main_thread::run()
{
    while(1)
    {
        //qDebug() << trackUpdate;
        if(trackUpdate>0)
        {
            trackUpdate= 0;
            emit updateAudioTrack();
        }
//        int ret =GetGpioStatus("/proc/rp_gpio/input_volume_mute");
//        if(ret==1)
//        {
//            qDebug() << "/proc/rp_gpio/input_volume_mute  =  " <<ret;

//            emit DebugSignal();
//        }


        usleep(1000*1);
    }
}

bool main_thread::isConnected()
{
    return manager->isOnline();
}

int main_thread::GetGpioStatus(QString GPIO_fILE)
{
    QString filePath =GPIO_fILE;

    // 创建 QFile 对象并传入路径
    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << file.errorString();
        return -1;
    }

    QString get_data = file.readAll();
    file.close();

    return get_data.toInt(NULL,10);
}

void main_thread::checkNetworkStatus()
{
           if (isConnected() && isNetOk== false  )
           {
               qDebug() << "Device is connected to the network.";
               emit wlanConnected();
               isNetOk = true;
               qDebug() << "12312313123123123";
           }
           else
           {
               qDebug() << "Device is not connected to the network.";
               isNetOk = false;
           }
}
