#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QFile>
#include "bt_a2dp_sink.h"

#include <QMutex>
#include <QMutexLocker>

#include "btmanager/bt_test.h"

class Key_event : public QThread
{
    Q_OBJECT
public:
    Key_event();
    virtual void run();
    int GetGpioStatus(QString GPIO_fILE);
    bool SetGPIO(QString path, int value);

    QString blue_addr;
    bool isConnect;
signals:
    void fingerKeysig(bool);

private:
    // 假设你在类中声明了一个 QMutex 锁
    QMutex m_mutex;
    
};

#endif // KEY_EVENT_H
