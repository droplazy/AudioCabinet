#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "my_define.h"
#include "rotatingroundedlabel.h"

class qDbus;
class HttpClient;
class Btmanager_thread;
class main_thread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString convertDurationToTimeFormat(const QString &durationStr);
    void wifiLaunch();
    void DisposeOneWord(S_HTTP_RESPONE s_back);
    int DisposePciteureJson(S_HTTP_RESPONE s_back);

    void displayAlbumPicOnlabel(QByteArray imageData);
    void GetAlbumPicture(QString Artist);
    void GetOnewords();
    void GetDeviceIP();
    void GetWeatherOnIp();
    void DownloadAudioPctrue(QString url);
    void disposeIP(S_HTTP_RESPONE s_back);
    void disposeWeatherInfo(QString jsonString);
    QVector<WeatherData> getWeatherData(const QString &jsonString, QString &place);
public slots:
    void displayAudioMeta(S_Aduio_Meta *p_meta);
    //void DisposeHttpResult(S_HTTP_RESPONE);
    void DisposeHttpResult(S_HTTP_RESPONE s_back);
    void AlbumPicRotato();
    void DebugChache();
private:
    Ui::MainWindow *ui;
    main_thread *p_thread;
    HttpClient *p_http;
    AUDIO_STATUS a_sta= NO_AUIO;
    QString Playing_Artist;
    QString Playing_Album;
    QString localPlace;
    QString DeviceIP;
    bool badKeywords=false;
    RotatingRoundLabel *label;
};



#endif // MAINWINDOW_H
