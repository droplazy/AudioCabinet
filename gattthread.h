#ifndef GATTTHREAD_H
#define GATTTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "btmanager/include/bt_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>



class gattthread:public QThread
{
    Q_OBJECT
public:
    virtual void run();
    gattthread();
    void handleWiFiConfiguration(const QJsonObject &dataObj);
    void handleFormat(const QString &data);
    void handleFingerprintEnrollment(const QString &data);
    void parseJson(const QString &jsonString);
signals:
    void wificonfigureupdate();
    void enrollFinger();
    void deviceFormat();

};

#endif // GATTTHREAD_H
