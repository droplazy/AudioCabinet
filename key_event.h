#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QFile>
#include "bt_a2dp_sink.h"


class Key_event:public QThread
{
    Q_OBJECT
public:
    Key_event();
    virtual void run();
 /*   QFile Key_UP_file();
    QFile Key_Middle_file();
    QFile Key_Down_file();*/
    int GetGpioStatus(QString GPIO_fILE);

    QString blue_addr;
    bool isConnect;
};

#endif // KEY_EVENT_H
