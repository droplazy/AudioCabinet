#ifndef GATTTHREAD_H
#define GATTTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "btmanager/include/bt_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "my_define.h"
#include <QElapsedTimer>

class gattthread : public QThread
{



    Q_OBJECT
public:
    virtual void run();
    gattthread();
    void handleWiFiConfiguration(const QJsonObject &dataObj);
    void handleFormat(const QString &data);
    void handleFingerprintEnrollment(const QString &data);
    void parseJson(const QString &jsonString);

    QString messageId ;
    int Respone_handle =-1;
    void ResponeGatt(char *data);
    GATT_MASSAGE GATT_STATE =GATT_MASSAGE::GATT_IDLE;
    void ResponeGattWIFICFG(bool res);
    QJsonObject generateJson(int resultId, const QString &operationType);

    QElapsedTimer timer_wificfg; // 创建计时器

signals:
    void wificonfigureupdate();
    void enrollFinger();
    void deviceFormat();
};

#endif // GATTTHREAD_H
