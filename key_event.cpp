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
    static QElapsedTimer fingerPressTimer; // 新增一个计时器用于消抖

    bool key_finger = false;

    // 初始化计时器（仅在首次使用时）
    if (!mutePressTimer.isValid())
    {
        mutePressTimer.start(); // 启动计时器
    }

    if (!addPressTimer.isValid())
    {
        addPressTimer.start(); // 启动计时器
    }

    if (!reducePressTimer.isValid())
    {
        reducePressTimer.start(); // 启动计时器
    }

    if (!fingerPressTimer.isValid())
    {
        fingerPressTimer.start(); // 启动指纹键消抖计时器
    }

    bool mutePress = false;
    qDebug() << "START!!!!!";
    while (1)
    {

        // 检查静音键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_mute"))
        {
            if (mutePressTimer.elapsed() >= 150 && mutePress == false)
            { // 每300ms切换静音状态
                volume_scale = -volume_scale;
                qDebug() << blue_addr << "mute: " << volume_scale << "mutePressTimer.elapsed() " << mutePressTimer.elapsed() << "/proc/rp_gpio/input_volume_mute";
                mutePress = true;
            }
        }
        else
        {
            mutePressTimer.restart(); // 松开时重置时间
            mutePress = false;
        }

        // 检查减音量键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_reduce"))
        {
            if (reducePressTimer.elapsed() > 150)
            {
                volume_scale = abs(volume_scale);
                volume_scale -= 1;
                if (volume_scale < 0)
                {
                    volume_scale = 0;
                }
                qDebug() << blue_addr << "volume : " << volume_scale << "reducePressTimer.elapsed() " << reducePressTimer.elapsed() << "/proc/rp_gpio/input_volume_reduce";
                reducePressTimer.restart(); // 重置时间，继续检测
            }
        }
        else
        {
            reducePressTimer.restart(); // 松开时重置时间
        }

        // 检查加音量键
        if (!GetGpioStatus("/proc/rp_gpio/input_volume_add"))
        {
            if (addPressTimer.elapsed() > 150)
            {
                volume_scale = abs(volume_scale);
                volume_scale += 1;
                if (volume_scale > 20)
                {
                    volume_scale = 20;
                }
                qDebug() << blue_addr << "volume : " << volume_scale << "addPressTimer.elapsed() " << addPressTimer.elapsed() << "/proc/rp_gpio/input_volume_add";
                addPressTimer.restart(); // 重置时间，继续检测
            }
        }
        else
        {
            addPressTimer.restart(); // 松开时重置时间
        }

        // 消抖处理：检查指纹键
        if (GetGpioStatus("/proc/rp_gpio/input_finger") != key_finger)
        {
            if (GetGpioStatus("/proc/rp_gpio/input_finger") == 0 || (GetGpioStatus("/proc/rp_gpio/input_finger") && fingerPressTimer.elapsed() > 200))
            { // 松开的时候立刻发送信号   按下的时消抖一段延时
                key_finger = GetGpioStatus("/proc/rp_gpio/input_finger");
                emit fingerKeysig(key_finger);
               qDebug() << "emit ! debug  todo " << fingerPressTimer.elapsed() << "key_finger" << key_finger;
                fingerPressTimer.restart(); // 重置消抖计时器
            }
        }
        else
        {
            fingerPressTimer.restart(); // 松开时重置时间
        }

        // 消抖处理：检查指纹键
        if (GetGpioStatus("/proc/rp_gpio/output_sd") != sd_status)
        {

            SetGPIO("/proc/rp_gpio/output_sd", sd_status);
#if 1
            qDebug() << "功放状态:" << sd_status << "实际状态 " << GetGpioStatus("/proc/rp_gpio/output_sd");
#endif
        }
        //   qDebug() << "fingerPressTimer.elapsed()" << fingerPressTimer.elapsed();
        usleep(50 * 1000); // 延迟50ms，避免过高的CPU占用
    }
}

int Key_event::GetGpioStatus(QString GPIO_fILE)
{
    // 获取锁，确保其他线程不能同时访问
    QMutexLocker locker(&m_mutex);

    QString filePath = GPIO_fILE;

    // 创建 QFile 对象并传入路径
    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 错误处理
        return -1;
    }

    QString get_data = file.readAll();
    file.close();

    return get_data.toInt(NULL, 10);
}
bool Key_event::SetGPIO(QString path, int value)
{
    // 获取锁，确保其他线程不能同时访问
    QMutexLocker locker(&m_mutex);

    // 打开文件进行读取
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug() << "无法打开文件!" << path;
        return false;
    }

    // 将新的值转化为字节并写入文件
    QByteArray newData = QByteArray::number(value) + '\n'; // 需要添加换行符以匹配文件格式
    qint64 bytesWritten = -1;
    QByteArray writtenData;

    // 使用QElapsedTimer来测量时间
    QElapsedTimer timer;
    timer.start();

    // 尝试直到1.5秒或成功写入
    while (timer.elapsed() < 1500)
    { // 1.5秒内循环
        // 先读取文件内容
        file.seek(0); // 移动文件指针到文件开头
        QByteArray currentData = file.readAll();
        qDebug() << "当前文件内容: " << currentData;

        // 写入新的值
        file.seek(0); // 移动文件指针到文件开头
        bytesWritten = file.write(newData);
        if (bytesWritten == -1)
        {
            qDebug() << "写入失败，等待重试...";
        }
        else
        {
            // 清空缓存并验证
            file.flush();
            file.seek(0); // 移动文件指针到文件开头
            writtenData = file.readAll();
            qDebug() << "写入后的文件内容: " << writtenData;

            // 校验写入的内容是否正确
            if (writtenData.trimmed() == newData.trimmed())
            {
                qDebug() << "写入成功!";
                file.close();
                return true;
            }
            else
            {
                qDebug() << "写入失败，验证不一致，继续尝试!";
            }
        }

        // 短暂的等待，以避免CPU占用过高
        QThread::msleep(50); // 暂停50毫秒
    }

    // 超过1.5秒后退出循环，返回失败
    file.close();
    qDebug() << "尝试1.5秒后失败!";
    return false;
}