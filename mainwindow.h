#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QMediaPlayer>
#include "my_define.h"
#include "rotatingroundedlabel.h"
#include <QDateTime>
#define  PULLUP_ELCLOCK    do{ system("echo 1 > /proc/rp_gpio/output_lock");} while(0)
#define  PULLDOWN_ELCLOCK    do{ system("echo 0 > /proc/rp_gpio/output_lock");} while(0)
#define RotationLabel 1



class qDbus;
class HttpClient;
class Btmanager_thread;
class main_thread;
class FingerThread;
class Key_event;
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
    int  DisposePciteureJson(S_HTTP_RESPONE s_back);

    void displayAlbumPicOnlabel(QByteArray imageData);
    void GetAlbumPicture(QString Artist, QString page, QString limit);
    void GetOnewords();
    void GetDeviceIP();
    void GetWeatherOnIp();
    void DownloadAudioPctrue(QString url);
    void disposeIP(S_HTTP_RESPONE s_back);
    void disposeWeatherInfo(QString jsonString);
    QVector<WeatherData> getWeatherData(const QString &jsonString, QString &place);
    void GetHotSearch();
    void DisposeHotSearch(S_HTTP_RESPONE s_back);
    void GetDateToday();
    void DisposeDate(S_HTTP_RESPONE s_back);
    void disposeWeathertoday(S_HTTP_RESPONE s_back);
    void GetWeatherToday();
    void initSepctrum();
    void CreatSpectrum();
    void smoothData(double spectrumMeta[], int length, double smoothingFactor);
  //  double calculateAverage(const QVector<double>& arr);
      void flushNetUI();

public slots:
    void checkNetworkStatus();
    void displayAudioMeta();
    void UserAddFinger();
    void displaySpectrum();
    void displaySpectrumFall();
    //void DisposeHttpResult(S_HTTP_RESPONE);
    void DisposeHttpResult(S_HTTP_RESPONE s_back);
    void AlbumPicRotato();
    void DebugChache();
    void ElcLockOption();
    int GetGpioStatus(QString GPIO_fILE);
    void setPlayProgress(int current );
    void updateDisplayTime();
    void SaveRealsedLock();
private:
    Ui::MainWindow *ui;
    main_thread *p_thread;
    FingerThread *p_finger;
    Key_event *p_keyevent;
    HttpClient *p_http;
  //  btmg_avrcp_play_state_t a_sta= BTMG_AVRCP_STOP;
    QString Playing_Artist="ZWDZ_NONE";
    QString Playing_Album="ZWDZ_NONE";
    QString localPlace;
    QString DeviceIP;
    bool badKeywords=false;
    RotatingRoundLabel *label_around;
    int picSearchCnt=0;
    int picGetcnt=0;
    int currentPosition =0;
    int positonoffset =0;//平滑进度条
    int offsetReduce =5;//平滑进度条
    QStringList getUrl;
    QDateTime dateTime;
    QLabel *labels_top[60];
    QLabel *labels_bottom[60];
    QLabel *label_progressbar_bottom;
    QLabel *label_progressbar_top;
    QLabel *label_aumblePic;
    int  fallspeed[60]={0};
    int fallCount[60]={0};
};



#endif // MAINWINDOW_H
