#include "key_event.h"

Key_event::Key_event()
{

}

void Key_event::run()
{

    while(1)
    {

        // 增加音量
        if(GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
        {//realse

        }
        else if (!GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
        {
            usleep(300 *1000);
            if (!GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
                    {
            volume_scale += 10;
            // 限制 volume_scale 在 0 到 100 之间
            if (volume_scale > 100) {
                volume_scale = 100;
            }
            qDebug() << blue_addr << "volume : " << volume_scale;
            }
        }

        // 降低音量
        if(GetGpioStatus("/proc/rp_gpio/input_volume_reduce"))
        {//realse

        }
        else if (!GetGpioStatus("/proc/rp_gpio/input_volume_reduce"))
        {
            usleep(300 *1000);
            if (!GetGpioStatus("/proc/rp_gpio/input_volume_reduce"))
                    {
            volume_scale -= 10;
            // 限制 volume_scale 在 0 到 100 之间
            if (volume_scale < 0) {
                volume_scale = 0;
            }
            qDebug() << blue_addr << "volume : " << volume_scale;
            }
        }


        if(GetGpioStatus("/proc/rp_gpio/input_volume_add"))
        {//realse

        }
        else if (!GetGpioStatus("/proc/rp_gpio/input_volume_add"))
        {

        }
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
