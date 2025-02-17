#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QFile>



class main_thread :public QThread
{
    Q_OBJECT
public:
    main_thread();
    virtual void run();
    bool isConnected();
    int GetGpioStatus(QString GPIO_fILE);


public slots:
    void checkNetworkStatus();

signals:
    void wlanConnected();
    void updateAudioTrack();
    void DebugSignal();

private:
    bool isNetOk = false;
     QNetworkConfigurationManager *manager;
};

#endif // MAIN_THREAD_H
