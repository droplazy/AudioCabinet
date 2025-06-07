#include "key_event.h"
#include <QTime>

#include <QElapsedTimer>
#include <QDebug>

Key_event::Key_event()
{

}

void Key_event::run()
{
    // 用于记录按键按下的时间
    static QElapsedTimer mutePressTimer;
    static QElapsedTimer addPressTimer;
    static QElapsedTimer reducePressTimer;

    // 初始化计时器（仅在首次使用时）
    if (!mutePressTimer.isValid()) {
        mutePressTimer.start();  // 启动计时器
    }

    if (!addPressTimer.isValid()) {
        addPressTimer.start();  // 启动计时器
    }

    if (!reducePressTimer.isValid()) {
        reducePressTimer.start();  // 启动计时器
    }
    bool mutePress = false; 
    qDebug() << "START!!!!!";
    while(1)
    {
       // qDebug() << "wow : " <<mutePressTimer.elapsed();
        // 检查静音键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
        {
       //     qDebug()<<"input_volume_mute.elapsed() " << mutePressTimer.elapsed() << GetGpioStatus("/proc/rp_gpio/input_volume_mute");

            if (mutePressTimer.elapsed() == 300 && mutePress ==false) {  // 每300ms切换静音状态
                volume_scale = -volume_scale;
                qDebug() << blue_addr << "mute: " << volume_scale << "mutePressTimer.elapsed() " << mutePressTimer.elapsed() << "/proc/rp_gpio/input_volume_mute";
             //   mutePressTimer.restart();  // 重置时间，继续检测
                 mutePress = true;
            }
        }
        else {
            mutePressTimer.restart(); // 松开时重置时间
            mutePress = false;
            /*if (mutePress)
           {
           usleep(300 *1000);
            if (GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
            mutePressTimer.restart(); // 松开时重置时间
           mutePress=false;
           }*/
        }
#if 1
        // 检查减音量键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_reduce"))
        {
        //    qDebug()<<"reducePressTimer.elapsed() " << reducePressTimer.elapsed() << GetGpioStatus("/proc/rp_gpio/input_volume_reduce");
            if (reducePressTimer.elapsed() > 300) {
                // 每300ms减少音量
                volume_scale = abs(volume_scale);
                volume_scale -= 1;
                // 限制 volume_scale 在 0 到 100 之间
                if (volume_scale < 0) {
                    volume_scale = 0;
                }
                qDebug() << blue_addr << "volume : " << volume_scale << "reducePressTimer.elapsed() " << reducePressTimer.elapsed() << "/proc/rp_gpio/input_volume_reduce";
                reducePressTimer.restart(); // 重置时间，继续检测
            }
        }
        else {
            reducePressTimer.restart(); // 松开时重置时间
        }

        // 检查加音量键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_add"))
        {
         //   qDebug()<<"addPressTimer.elapsed() " << addPressTimer.elapsed() << GetGpioStatus("/proc/rp_gpio/input_volume_add");
            if (addPressTimer.elapsed() > 300) {
                // 每300ms增加音量
                volume_scale = abs(volume_scale);
                volume_scale += 1;
                // 限制 volume_scale 在 0 到 10 之间
                if (volume_scale > 10) {
                    volume_scale = 10;
                }
                qDebug() << blue_addr << "volume : " << volume_scale << "addPressTimer.elapsed() " << addPressTimer.elapsed() << "/proc/rp_gpio/input_volume_add";
                addPressTimer.restart();  // 重置时间，继续检测
            }
        }
        else {
            addPressTimer.restart(); // 松开时重置时间
        }
        #endif 
    }
}





int Key_event::GetGpioStatus(QString GPIO_fILE)
{
    QString filePath =GPIO_fILE;

    // 创建 QFile 对象并传入路径
    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
     //   qDebug() << "无法打开文件:" << file.errorString();
        return -1;
    }

    QString get_data = file.readAll();
    file.close();

    return get_data.toInt(NULL,10);
}
