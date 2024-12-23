#ifndef QDUS_H
#define QDUS_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QString>
#include <QVariantMap>

class audio_info;
class MainWindow;
class qDbus: public QThread
{
    Q_OBJECT
public:
    qDbus();
    virtual void run();
    QString GetPairingStatus();
    bool VolumeDown();
    void GetBLueToothPairing();
    int QprocessCommand(QString program, QString &output, QStringList arguments);
    void QprocessBlueToothCtl();
    void TrustScanDevices();
    void CheckTrustDevicesList();

    MainWindow *p_main;

private:
    QString HwId ;
    QString BLname;
    QString Alias;//
    QString Trusted;
    QStringList Trsutlist;
    QProcess bluetoothProcess;
    audio_info *p_audio;
    bool Paired;
    bool isblueAlsaTurnon =false;
    bool isMETAON =false;
    bool isBlconnected = false;
    bool CheckBlueDevice(QString BlueInfo);

};

#endif // QDUS_H
