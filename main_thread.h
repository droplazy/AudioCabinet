#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QFile>
#include <complex.h>
#include <math.h>
#include <fftw3.h>
#include <cmath>
#include <vector>




class main_thread :public QThread
{
    Q_OBJECT
public:
    main_thread();
    virtual void run();
    bool isConnected();
    int GetGpioStatus(QString GPIO_fILE);
    void SpectrumMetaData();
    void calculatePowerSpectrum(const char* pcmData, int sampleRate, int N);

    double spectrumMeta[60]={0};
    void recheckNetwork();
        bool isNetOk = false;

public slots:
    void checkNetworkStatus();

signals:
    void wlanConnected();
    void updateAudioTrack();
    void DebugSignal();

private:
   //  QNetworkConfigurationManager *manager;
};

#endif // MAIN_THREAD_H
